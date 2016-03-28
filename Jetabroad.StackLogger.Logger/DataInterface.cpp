#include "PCH.h"
#include "DataInterface.h"
#include "VectoredExceptionHandler.h"
#include "DumpedData.h"
#include "TLS.h"

static INIT_ONCE g_initialiseFlag = INIT_ONCE_STATIC_INIT;

// Always return TRUE from this function to prevent subsequent running.
static BOOL WINAPI OneTimeInitialisation(PINIT_ONCE /* InitOnce */, PVOID /* Parameter */, PVOID *Context)
{
	try
	{
		InitialiseVectoredExceptionHandler();
	}
	catch (CAtlException& e)
	{
		*Context = reinterpret_cast<PVOID>(LONG_PTR(e)); // Use LONG_PTR to prevent "C4312: 'reinterpret_cast': conversion from 'HRESULT' to 'PVOID' of greater size."
		return TRUE;
	}
	catch (...)
	{
		*Context = reinterpret_cast<PVOID>(E_FAIL);
		return TRUE;
	}

	*Context = S_OK;
	return TRUE;
}

DataInterface::DataInterface()
{
	ATLTRACE2(_T("[%u] DataInterface()"), GetCurrentThreadId());
}

DataInterface::~DataInterface()
{
	ATLTRACE2(_T("[%u] ~DataInterface()"), GetCurrentThreadId());
}

HRESULT DataInterface::FinalConstruct()
{
	LONG_PTR hr;

	if (!InitOnceExecuteOnce(&g_initialiseFlag, OneTimeInitialisation, NULL, reinterpret_cast<PVOID *>(&hr)))
		return AtlHresultFromLastError();

	return static_cast<HRESULT>(hr);
}

// Place initialisation here. Don't do anything that interfere with DllMain.
// See https://msdn.microsoft.com/en-us/library/windows/desktop/dn633971(v=vs.85).aspx for details.
VOID DataInterface::Init()
{
}

// Place finalisation here. Don't do anything that interfere with DllMain.
// See https://msdn.microsoft.com/en-us/library/windows/desktop/dn633971(v=vs.85).aspx for details.
BOOL DataInterface::Term()
{
	if (!FinaliseVectoredExceptionHandler())
		return FALSE;

	return TRUE;
}

HRESULT DataInterface::get_HasLastDumped(VARIANT_BOOL *result)
{
	if (!result)
		return E_POINTER;

	*result = GetThreadLastExceptionData() ? VARIANT_TRUE : VARIANT_FALSE;

	return S_OK;
}

HRESULT DataInterface::get_HasLastExceptionDumpingLogs(VARIANT_BOOL *result)
{
	if (!result)
		return E_POINTER;

	if (g_pTls->Exists(TLSKEY_LAST_EXCEPTION_DUMPING_LOG))
		*result = VARIANT_TRUE;
	else
		*result = VARIANT_FALSE;

	return S_OK;
}

HRESULT DataInterface::ClearLastDumped()
{
	ClearThreadLastExceptionData();
	return S_OK;
}

HRESULT DataInterface::QueryLastDumped(IDumpedData **data)
{
	if (!data)
		return E_POINTER;

	auto pData = GetThreadLastExceptionData();
	if (!pData)
		return E_FAIL;

	return pData.CopyTo(data);
}

HRESULT DataInterface::QueryLastExceptionDumpingLogs(SAFEARRAY **logs)
{
	if (!logs)
		return E_POINTER;

	if (!g_pTls->Exists(TLSKEY_LAST_EXCEPTION_DUMPING_LOG))
		return E_FAIL;

	auto pLogs = static_cast<CAutoPtrTlsValue< CComSafeArray<LPUNKNOWN> > *>(g_pTls->Get(TLSKEY_LAST_EXCEPTION_DUMPING_LOG))->GetValue();
	return pLogs->CopyTo(logs);
}
