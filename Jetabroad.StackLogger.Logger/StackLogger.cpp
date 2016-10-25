#include "PCH.h"
#include "StackLogger.h"

#include "DAC.h"
#include "DACTargetProvider.h"
#include "ExceptionDumper.h"
#include "OperationLog.h"

////////////////////////////////////////////////////////////////////////////////
// StackLogger

StackLogger::StackLogger() :
	enabled(true),
	logging(false),
	veh(nullptr)
{
}

StackLogger::~StackLogger()
{
	// There is no documents to tell whether RemoveVectoredExceptionHandler() is synchronous or asynchronous on MSDN.
	// So we assume it is synchronous. That mean when it return no one will ever call the installed handler again.
	if (veh && !RemoveVectoredExceptionHandler(veh))
		AtlThrowLastWin32();
}

HRESULT StackLogger::FinalConstruct()
{
	try
	{
		// Initialise DAC.
		dmod = load_dac();

		// Install VEH Handler.
		argument_description vehargs[] =
		{
			{ argument_type::pointer }
		};

		vthunk.create(this, &StackLogger::VectoredExceptionHandler, vehargs, _countof(vehargs));

		veh = AddVectoredExceptionHandler(FALSE, reinterpret_cast<PVECTORED_EXCEPTION_HANDLER>(vthunk.get_thunk()));
		if (!veh)
			return E_FAIL;
	}
	catch (CAtlException& e)
	{
		return e;
	}
	catch (std::system_error& e)
	{
		return HRESULT_FROM_WIN32(e.code().value());
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT StackLogger::get_Enabled(BOOL *enabled)
{
	if (!enabled)
		return E_POINTER;

	*enabled = this->enabled ? TRUE : FALSE;

	return S_OK;
}

HRESULT StackLogger::put_Enabled(BOOL enabled)
{
	this->enabled = enabled ? true : false;
	return S_OK;
}

HRESULT StackLogger::get_InternalLoggingEnabled(BOOL *enabled)
{
	if (!enabled)
		return E_POINTER;

	*enabled = logging ? TRUE : FALSE;

	return S_OK;
}

HRESULT StackLogger::put_InternalLoggingEnabled(BOOL enabled)
{
	logging = enabled ? true : false;
	return S_OK;
}

HRESULT StackLogger::get_MaximumStackDepth(LONG *depth)
{
	if (!depth)
		return E_POINTER;

	{
		std::lock_guard<std::shared_mutex> lock(dconf_mtx);
		*depth = dconf.max_frames;
	}

	return S_OK;
}

HRESULT StackLogger::put_MaximumStackDepth(LONG depth)
{
	if (depth < 0)
		return E_INVALIDARG;

	{
		std::lock_guard<std::shared_mutex> lock(dconf_mtx);
		dconf.max_frames = depth;
	}

	return S_OK;
}

HRESULT StackLogger::get_HasData(BOOL *result)
{
	if (!result)
		return E_POINTER;

	try
	{
		auto data = tls.get_value();
		*result = (data && data->stack_data) ? TRUE : FALSE;
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT StackLogger::get_HasOperationLogs(BOOL *result)
{
	if (!result)
		return E_POINTER;

	try
	{
		auto data = tls.get_value();
		*result = (data && data->logs.GetCount()) ? TRUE : FALSE;
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT StackLogger::get_Data(IStackData **data)
{
	if (!data)
		return E_POINTER;

	try
	{
		auto thr_data = tls.get_value();
		if (!thr_data || !thr_data->stack_data)
			return E_FAIL;

		return thr_data->stack_data.CopyTo(data);
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT StackLogger::get_OperationLogs(SAFEARRAY **logs)
{
	if (!logs)
		return E_POINTER;

	try
	{
		// Get per-thread data.
		auto data = tls.get_value();
		if (!data)
			return E_FAIL;

		// Copy logs to SAFEARRAY.
		LONG nlog = static_cast<LONG>(data->logs.GetCount());
		if (!nlog)
			return E_FAIL;

		CComSafeArray<LPUNKNOWN> arr(nlog);

		for (LONG i = 0; i < nlog; i++)
		{
			auto hr = arr.SetAt(i, data->logs[i]);
			if (FAILED(hr))
				return hr;
		}

		// Return logs.
		*logs = arr.Detach();
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT StackLogger::ClearData()
{
	try
	{
		auto data = tls.get_value();
		if (!data)
			return S_OK;
		
		data->stack_data.Release();
		return S_OK;
	}
	catch (...)
	{
		return E_FAIL;
	}
}

HRESULT StackLogger::ClearOperationLogs()
{
	try
	{
		auto data = tls.get_value();
		if (!data)
			return S_OK;
		
		data->logs.RemoveAll();
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}

LONG StackLogger::VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	// Prevent recursive running. This make it safe to be able to throw exceptions after this line.
	// If we don't check for this, when exception occur it will call this function again and again until stack is overflow.
	// FIXME: Find a better way to ensure it not throw exception before we are in a safe situation.
	auto tdata = tls.get_value();
	if (!tdata)
	{
		tdata = std::make_shared<thread_data>();
		tls.set_value(tdata);
	}

	if (tdata->in_veh)
		return EXCEPTION_CONTINUE_SEARCH;

	tdata->in_veh = true;

	// Start Dumping.
	try
	{
		if (enabled)
		{
			// Clean old data before append a new one.
			ClearData();
			ClearOperationLogs();

			// Create DAC.
			auto dtarget = create_com<DACTargetProvider>();

			dtarget->SetLogger(this);
			dtarget->SetException(pExceptionInfo);

			auto dac = create_dac(dmod, dtarget.p);

			// Dump.
			exception_dumper::config dconf;

			{
				std::shared_lock<std::shared_mutex> lock(dconf_mtx);
				dconf = this->dconf;
			}

			exception_dumper dumper(dconf, dac, GetCurrentThreadId(), *this);
			tdata->stack_data = dumper.run();
		}
	}
	catch (CAtlException& e)
	{
		WriteLog(L"Failed to dump exception details: 0x%X.", e);
	}
	catch (std::exception& e)
	{
		WriteLog(L"Failed to dump exception details: 0x%hs.", e.what());
	}
	catch (...)
	{
		WriteLog(L"Failed to dump exception details: Unknown Error.");
	}

	tdata->in_veh = false;

	return EXCEPTION_CONTINUE_SEARCH;
}

VOID StackLogger::WriteLog(LPCWSTR pszFormat, ...) noexcept
{
	try
	{
		if (!logging)
			return;

		// Format message.
		CString msg;
		va_list args;

		va_start(args, pszFormat);
		try
		{
			msg.FormatV(pszFormat, args);
		}
		catch (...)
		{
			va_end(args);
			throw;
		}
		va_end(args);

		// Create IOperationLog.
		auto log = create_com<OperationLog>();

		log->SetMessage(msg);
		log->SetTime(COleDateTime::GetCurrentTime());

		// Add log to list.
		auto tdata = tls.get_value();
		if (!tdata)
		{
			tdata = std::make_shared<thread_data>();
			tls.set_value(tdata);
		}

		tdata->logs.Add(log);
	}
	catch (...)
	{
		// Swallow.
	}
}
