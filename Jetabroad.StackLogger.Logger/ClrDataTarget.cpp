#include "PCH.h"
#include "ClrDataTarget.h"
#include "TLS.h"
#include "ExceptionDumpingMessageCollection.h"
#include "GlobalConfiguration.h"

static CExceptionDumpingMessageCollection * GetCurrentLogger()
{
	CString strCurrentLoggerKey;
	strCurrentLoggerKey.Format(TLSKEY_CURRENT_LOGGER, GetCurrentThreadId());
	auto pTlsValue = static_cast<CGeneralTlsValue<CExceptionDumpingMessageCollection *> *>(g_pTls->Get(strCurrentLoggerKey));
	return pTlsValue ? pTlsValue->GetValue() : NULL;
}

static void AppendCurrentLog(PCWSTR pszFormat, ...)
{
	if (!g_pConfig->GetLogExceptionDumping())
		return;

	CString strMessage;
	va_list args;

	va_start(args, pszFormat);
	try
	{
		strMessage.FormatV(pszFormat, args);
	}
	catch (...)
	{
		va_end(args);
		throw;
	}
	va_end(args);

	auto pLog = GetCurrentLogger();
	if (pLog)
		pLog->Append(L"%s", strMessage);
	else
		ATLTRACE2(L"[%u] %s\n", GetCurrentThreadId(), strMessage);
}

ClrDataTarget::ClrDataTarget()
{
	ATLTRACE2(_T("Initialise ClrDataTarget"));
}

ClrDataTarget::~ClrDataTarget()
{
	ATLTRACE2(_T("Finalise ClrDataTarget"));
}

HRESULT ClrDataTarget::GetMachineType(ULONG32 *machineType)
{
	AppendCurrentLog(L"CLR is asking for machine type.");

	if (!machineType)
	{
		AppendCurrentLog(L"Cannot return the machine type since CLR send an invalid pointer.");
		return E_POINTER;
	}

#if defined(_M_IX86)
	*machineType = IMAGE_FILE_MACHINE_I386;
#elif defined(_M_AMD64)
	*machineType = IMAGE_FILE_MACHINE_AMD64;
#else
#error Target machine is not supported.
#endif

	return S_OK;
}

HRESULT ClrDataTarget::GetPointerSize(ULONG32 *pointerSize)
{
	AppendCurrentLog(L"CLR is asking for the size of pointer.");

	if (!pointerSize)
	{
		AppendCurrentLog(L"Cannot return the pointer size since CLR send an invalid pointer.");
		return E_POINTER;
	}

	*pointerSize = sizeof(PVOID);
	return S_OK;
}

HRESULT ClrDataTarget::GetImageBase(LPCWSTR imagePath, CLRDATA_ADDRESS *baseAddress)
{
	AppendCurrentLog(L"CLR is asking for image base of module %s.", imagePath);

	if (!imagePath || !baseAddress)
	{
		AppendCurrentLog(L"Cannot return image base since CLR send an invalid pointer.");
		return E_POINTER;
	}

	auto hModule = GetModuleHandle(imagePath);
	if (!hModule)
	{
		auto hr = AtlHresultFromLastError();
		AppendCurrentLog(L"Failed to get image base for module %s: 0x%X.", imagePath, hr);
		return hr;
	}

	*baseAddress = reinterpret_cast<CLRDATA_ADDRESS>(hModule);
	return S_OK;
}

HRESULT ClrDataTarget::ReadVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesRead)
{
	AppendCurrentLog(L"CLR is asking for %u bytes at 0x%IX.", bytesRequested, address);

	if (!buffer || !bytesRead)
	{
		AppendCurrentLog(L"Cannot return %u bytes since CLR send an invalid pointer.", bytesRequested);
		return E_POINTER;
	}

	__try
	{
		CopyMemory(buffer, reinterpret_cast<PVOID>(address), bytesRequested);
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		AppendCurrentLog(L"Access violation while reading memory at 0x%IX.", address);
		return E_ACCESSDENIED;
	}

	*bytesRead = bytesRequested;
	return S_OK;
}

HRESULT ClrDataTarget::WriteVirtual(CLRDATA_ADDRESS address, BYTE *buffer, ULONG32 bytesRequested, ULONG32 *bytesWritten)
{
	AppendCurrentLog(L"CLR is asking to write %u bytes at 0x%IX.", bytesRequested, address);

	if (!buffer || !bytesWritten)
	{
		AppendCurrentLog(L"Cannot write since CLR send an invalid pointer.");
		return E_POINTER;
	}

	__try
	{
		CopyMemory(reinterpret_cast<PVOID>(address), buffer, bytesRequested);
	}
	__except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	{
		AppendCurrentLog(L"Access violation while writing %u bytes at 0x%IX.", bytesRequested, address);
		return E_ACCESSDENIED;
	}

	*bytesWritten = bytesRequested;
	return S_OK;
}

HRESULT ClrDataTarget::GetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS * /* value */)
{
	AppendCurrentLog(L"CLR is asking for TLS value at index %u for thread %u.", index, threadID);
	return E_NOTIMPL;
}

HRESULT ClrDataTarget::SetTLSValue(ULONG32 threadID, ULONG32 index, CLRDATA_ADDRESS /* value */)
{
	AppendCurrentLog(L"CLR is asking to set TLS value at index %u for thread %u.", index, threadID);
	return E_NOTIMPL;
}

HRESULT ClrDataTarget::GetCurrentThreadID(ULONG32 * /* threadID */)
{
	AppendCurrentLog(L"CLR is asking for current thread's identifier.");
	return E_NOTIMPL;
}

HRESULT ClrDataTarget::GetThreadContext(ULONG32 threadID, ULONG32 contextFlags, ULONG32 contextSize, BYTE *context)
{
	AppendCurrentLog(L"CLR is asking for context of thread %u.", threadID);

	if (contextSize != sizeof(CONTEXT))
	{
		AppendCurrentLog(L"Cannot return thread's context since CLR send an invalid buffer.");
		return E_INVALIDARG;
	}
	
	auto pContext = reinterpret_cast<PCONTEXT>(context);

	CString strCurrentExceptionKey;
	strCurrentExceptionKey.Format(TLSKEY_CURRENT_EXCEPTION_POINTER, threadID);
	auto pTlsValue = static_cast<CGeneralTlsValue<PEXCEPTION_POINTERS> *>(g_pTls->Get(strCurrentExceptionKey));

	if (pTlsValue)
	{
		ATLTRACE2(_T("[%u] GetThreadContext() with available exception pointer"), GetCurrentThreadId());

		CopyMemory(pContext, pTlsValue->GetValue()->ContextRecord, contextSize);
		pContext->ContextFlags = contextFlags;
	}
	else
	{
		ATLTRACE2(_T("[%u] GetThreadContext() with current context"), GetCurrentThreadId());

		// Get a thread handle to dump context.
		auto hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, threadID);
		if (!hThread)
		{
			auto hr = AtlHresultFromLastError();
			ATLTRACE2(_T("[%u] GetThreadContext(%u, 0x%X, %u, 0x%IX) -> 0x%X"), GetCurrentThreadId(), threadID, contextFlags, contextSize, context, hr);
			return hr;
		}

		// Dump Context.
		pContext->ContextFlags = contextFlags;
		auto blSuccess = ::GetThreadContext(hThread, pContext);
		CloseHandle(hThread);

		if (!blSuccess)
		{
			auto hr = AtlHresultFromLastError();
			ATLTRACE2(_T("[%u] GetThreadContext(%u, 0x%X, %u, 0x%IX) -> 0x%X"), GetCurrentThreadId(), threadID, contextFlags, contextSize, context, hr);
			return hr;
		}
	}

	return S_OK;
}

HRESULT ClrDataTarget::SetThreadContext(ULONG32 threadID, ULONG32 /* contextSize */, BYTE * /* context */)
{
	AppendCurrentLog(L"CLR is asking for setting context for thread %u.", threadID);
	return E_NOTIMPL;
}

HRESULT ClrDataTarget::Request(ULONG32 reqCode, ULONG32 /* inBufferSize */, BYTE * /* inBuffer */, ULONG32 /* outBufferSize */, BYTE * /* outBuffer */)
{
	AppendCurrentLog(L"CLR is asking for a service %u.", reqCode);
	return E_NOTIMPL;
}

HRESULT ClrDataTarget::AllocVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags, ULONG32 protectFlags, CLRDATA_ADDRESS *virt)
{
	AppendCurrentLog(L"CLR is asking for %u bytes allocation at 0x%IX.", size, addr);

	if (!virt)
		return E_POINTER;

	auto pMemory = VirtualAlloc(reinterpret_cast<LPVOID>(addr), size, typeFlags, protectFlags);
	if (!pMemory)
		return AtlHresultFromLastError();

	*virt = reinterpret_cast<CLRDATA_ADDRESS>(pMemory);
	return S_OK;
}

HRESULT ClrDataTarget::FreeVirtual(CLRDATA_ADDRESS addr, ULONG32 size, ULONG32 typeFlags)
{
	AppendCurrentLog(L"CLR is asking to free %u bytes at 0x%IX.", size, addr);

#pragma warning(push)
#pragma warning(disable:28160) // Passing MEM_RELEASE and a non - zero dwSize parameter to VirtualFree is not allowed.
                               // We need to ignore this warning since we need to passthrough original behavior back to CLR.
	if (!VirtualFree(reinterpret_cast<LPVOID>(addr), size, typeFlags))
		return AtlHresultFromLastError();
#pragma warning(pop)

	return S_OK;
}
