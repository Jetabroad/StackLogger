// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
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
	max_data_entries(10),
	dmod(NULL),
	veh(nullptr),
	in_veh(0)
{
}

StackLogger::~StackLogger()
{
	// Uninstall VEH handler first.
	if (veh && !RemoveVectoredExceptionHandler(veh))
		AtlThrowLastWin32();

	// Then wait for current VEH handling to finish.
	while (InterlockedCompareExchange(&in_veh, -1, 0) != 0)
		Sleep(0);

	// Yield the execution to another thread to delay object destroying since it
	// possible to have another exception already fired before we do RemoveVectoredExceptionHandler().
	Sleep(0);
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

HRESULT StackLogger::get_MaximumDataEntries(LONG *number)
{
	if (!number)
		return E_POINTER;

	*number = max_data_entries;

	return S_OK;
}

HRESULT StackLogger::put_MaximumDataEntries(LONG number)
{
	if (number < 0)
		return E_INVALIDARG;

	max_data_entries = number;

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
		*result = (data && data->exceptions.GetCount()) ? TRUE : FALSE;
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

HRESULT StackLogger::get_AllData(SAFEARRAY **data)
{
	if (!data)
		return E_POINTER;

	try
	{
		auto tdata = tls.get_value();
		if (!tdata)
			return E_FAIL;

		auto ndata = static_cast<LONG>(tdata->exceptions.GetCount());
		if (!ndata)
			return E_FAIL;

		CComSafeArray<LPUNKNOWN> exceptions(ndata);
		auto pos = tdata->exceptions.GetHeadPosition();
		LONG i = 0;

		while (pos)
		{
			auto hr = exceptions.SetAt(i++, tdata->exceptions.GetNext(pos));
			if (FAILED(hr))
				return hr;
		}

		*data = exceptions.Detach();
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT StackLogger::get_Data(IStackData **data)
{
	if (!data)
		return E_POINTER;

	try
	{
		auto tdata = tls.get_value();
		if (!tdata || !tdata->exceptions.GetCount())
			return E_FAIL;

		return tdata->exceptions.GetHead().CopyTo(data);
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
		
		data->exceptions.RemoveAll();
	}
	catch (...)
	{
		return E_FAIL;
	}

	return S_OK;
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

VOID StackLogger::HandleProcessException(PEXCEPTION_POINTERS pExceptionInfo)
{
	// Prevent recursive running. This make it safe to be able to throw exceptions after this line.
	// If we don't check for this, when exception occur it will call this function again and again until stack is overflow.
	thread_local static bool reentrant;

	if (reentrant)
		return;

	reentrant = true;

	try
	{
		if (!enabled)
			goto Exit;

		// Prepare storage.
		auto tdata = tls.get_value();
		if (!tdata)
		{
			tdata = std::make_shared<thread_data>();
			tls.set_value(tdata);
		}

		// Prepare config.
		exception_dumper::config dconf;

		{
			// Lock inside this block only to reduce locking time.
			std::shared_lock<std::shared_mutex> lock(dconf_mtx);
			dconf = this->dconf;
		}

		// Clean old data before append a new one.
		ClearData();
		ClearOperationLogs();

		// Create DAC.
		auto dtarget = create_com<DACTargetProvider>();

		dtarget->SetLogger(this);
		dtarget->SetException(pExceptionInfo);

		auto dac = create_dac(dmod, dtarget.p);

		// Dump.
		try
		{
			exception_dumper dumper(dconf, dac, GetCurrentThreadId(), *this);
			tdata->exceptions.AddHead(dumper.run());
		}
		catch (...)
		{
			dac->Flush();
			throw;
		}

		dac->Flush();

		// Remove unused exception entries.
		auto nremove = static_cast<int>(tdata->exceptions.GetCount()) - max_data_entries;

		for (int i = 0; i < nremove; i++)
			tdata->exceptions.RemoveTailNoReturn();
	}
	catch (CAtlException& e)
	{
		ATLTRY(WriteLog(L"Failed to dump exception details: 0x%X.", e));
	}
	catch (std::exception& e)
	{
		ATLTRY(WriteLog(L"Failed to dump exception details: 0x%hs.", e.what()));
	}
	catch (...)
	{
		ATLTRY(WriteLog(L"Failed to dump exception details: Unknown Error."));
	}

	Exit:
	reentrant = false;
}

LONG StackLogger::VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	// Filter exceptions that we want to process.
	// It not safe to put any breakpoint before "if" statement in the below.
	// 0xE0434352 is exception code for CLR.
	auto excode = pExceptionInfo->ExceptionRecord->ExceptionCode;
	if (excode == STATUS_BREAKPOINT || excode == STATUS_SINGLE_STEP || excode != 0xE0434352)
		return EXCEPTION_CONTINUE_SEARCH;

	// Increase number of threads being inside VEH.
	for (;;)
	{
		auto cveh = in_veh;
		if (cveh == -1)
			return EXCEPTION_CONTINUE_SEARCH;

		if (InterlockedCompareExchange(&in_veh, cveh + 1, cveh) == cveh)
			break;
	}

	// Dump.
	auto err = GetLastError(); // Preserve last error code.
	HandleProcessException(pExceptionInfo);
	SetLastError(err);

	// Decrease number of threads being inside VEH.
	InterlockedDecrement(&in_veh);

	return EXCEPTION_CONTINUE_SEARCH;
}

VOID StackLogger::WriteLog(LPCWSTR pszFormat, ...)
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
