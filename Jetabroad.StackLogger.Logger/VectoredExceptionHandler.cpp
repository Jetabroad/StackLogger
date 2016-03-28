#include "PCH.h"
#include "VectoredExceptionHandler.h"
#include "DumpedData.h"
#include "TLS.h"
#include "ClrDataTarget.h"
#include "FrameData.h"
#include "MethodParameter.h"
#include "ArrayValue.h"
#include "StringValue.h"
#include "RawValue.h"
#include "NullValue.h"
#include "ExceptionDumpingMessageCollection.h"
#include "UnknownValue.h"
#include "GlobalConfiguration.h"

#ifdef _WIN64
#define CLR_DIRECTORY _T("Microsoft.NET\\Framework64\\v4.0.30319")
#else
#define CLR_DIRECTORY _T("Microsoft.NET\\Framework\\v4.0.30319")
#endif

static IXCLRDataProcess *g_pClrDataProcess;
static PVOID g_pVehHandle;
static CComAutoDeleteCriticalSection g_csClrDataProcess;

static CComPtr<IUnknown> DumpClrDataValue(IXCLRDataValue *pClrValue, CExceptionDumpingMessageCollection *pLog);

static CComPtr<IUnknown> DumpClrString(IXCLRDataValue *pClrString)
{
	HRESULT hr;

	// Create object to hold dumped string.
	CAutoPtr< CComObject<StringValue> > pValue;
	hr = CComObject<StringValue>::CreateInstance(&pValue.m_p);
	if (FAILED(hr))
		AtlThrow(hr);

	// Query the length of string first to determine required buffer.
	ULONG32 uLen;
	hr = pClrString->GetString(0, &uLen, NULL);
	if (FAILED(hr))
		AtlThrow(hr);

	// Query string.
	CStringW strValue;
	hr = pClrString->GetString(uLen, NULL, strValue.GetBufferSetLength(uLen));
	if (FAILED(hr))
		AtlThrow(hr);
	strValue.ReleaseBuffer();

	pValue->SetValue(strValue);
	return pValue.Detach();
}

static CComPtr<IUnknown> DumpClrRawValue(IXCLRDataValue *pClrValue)
{
	HRESULT hr;

	// Create object to hold dumped data.
	CAutoPtr< CComObject<RawValue> > pValue;
	hr = CComObject<RawValue>::CreateInstance(&pValue.m_p);
	if (FAILED(hr))
		AtlThrow(hr);

	// Query object size to determine required buffer.
	ULONG64 uLen;
	hr = pClrValue->GetSize(&uLen);
	if (FAILED(hr))
	{
		ATLTRACE2(_T("[%u] IXCLRDataValue::GetSize() -> 0x%X"), GetCurrentThreadId(), hr);
		AtlThrow(hr);
	}

	// Copy object data.
	CComPtr<IStream> pData;

	if (uLen > ULONG_MAX)
	{
		pData.Attach(SHCreateMemStream(NULL, 0));
		if (!pData)
			AtlThrow(E_OUTOFMEMORY);
	}
	else
	{
		CTempBuffer<BYTE> buffer;
		hr = pClrValue->GetBytes(static_cast<ULONG32>(uLen), NULL, buffer.Allocate(uLen));
		if (FAILED(hr))
			AtlThrow(hr);

		pData.Attach(SHCreateMemStream(buffer, static_cast<UINT>(uLen)));
		if (!pData)
			AtlThrow(E_OUTOFMEMORY);
	}

	pValue->SetValue(pData);
	return pValue.Detach();
}

static CComPtr<IUnknown> DumpClrArray(IXCLRDataValue *pClrArray, CExceptionDumpingMessageCollection *pLog)
{
	HRESULT hr;

	// Create object to hold dumped array.
	pLog->Append(L"Creating ArrayValue instance.");

	CAutoPtr< CComObject<ArrayValue> > pArray;
	hr = CComObject<ArrayValue>::CreateInstance(&pArray.m_p);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to create ArrayValue instance: 0x%X.", hr);
		AtlThrow(hr);
	}

	// Query array properties.
	// The maximum dimension for array is 32 (https://msdn.microsoft.com/en-us/library/system.array(v=vs.110).aspx).
	pLog->Append(L"Getting array properties.");

	ULONG32 uRank, uElements; // uRank is number of dimension. uElements is number of total elements.
	ULONG32 bounds[32]; // Number of bound for each dimension.
	LONG32 bases[32]; // Lower bounds of each dimension.

	hr = pClrArray->GetArrayProperties(&uRank, &uElements, _countof(bounds), bounds, _countof(bases), bases);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get array properties: 0x%X.", hr);
		AtlThrow(hr);
	}

	// TODO: Support multi-dimensional array and array that lower bound is not zero.
	pLog->Append(L"Creating CComSafeArray<LPUNKNOWN> instance.");

	CComSafeArray<LPUNKNOWN> values;

	if (uRank != 1 || bases[0] != 0)
	{
		hr = values.Create();
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to create CComSafeArray<LPUNKNOWN>: 0x%X.", hr);
			AtlThrow(hr);
		}

		pLog->Append(L"Array is multi-dimensional or starting index is not zero. Right now we does not support this kind of array.");
	}
	else
	{
		// Dump Elements.
		hr = values.Create(uElements);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to create CComSafeArray<LPUNKNOWN> with %u elements: 0x%X.", uElements, hr);
			AtlThrow(hr);
		}

		pLog->Append(L"Start dumping %u elements.", uElements);

		for (ULONG32 i = 0; i < uElements; i++)
		{
			pLog->Append(L"Getting element %u.", i);

			LONG32 indices[] = { static_cast<LONG32>(i) };
			CComPtr<IXCLRDataValue> pElement;
			hr = pClrArray->GetArrayElement(uRank, indices, &pElement);
			if (FAILED(hr))
			{
				pLog->Append(L"Failed to get element %u: 0x%X.", i, hr);
				AtlThrow(hr);
			}

			pLog->Append(L"Dumping element.");
			auto pValue = DumpClrDataValue(pElement, pLog);
			hr = values.SetAt(i, pValue);
			if (FAILED(hr))
			{
				pLog->Append(L"Failed to save dumped element: 0x%X.", hr);
				AtlThrow(hr);
			}
		}

		pLog->Append(L"Finished dumping %u elements.", uElements);
	}
	
	pArray->SetValues(values);
	return pArray.Detach();
}

static CComPtr<IUnknown> DumpClrDataValue(IXCLRDataValue *pClrValue, CExceptionDumpingMessageCollection *pLog)
{
	HRESULT hr;

	// Check for null value.
	pLog->Append(L"Getting object size.");

	ULONG64 uValueSize;
	hr = pClrValue->GetSize(&uValueSize);
	if (FAILED(hr))
	{
		if (hr != E_NOINTERFACE)
		{
			pLog->Append(L"Failed to get object size: 0x%X.", hr);
			AtlThrow(hr);
		}

		pLog->Append(L"Creating NullValue instance.");
		CAutoPtr< CComObject<NullValue> > pValue;
		hr = CComObject<NullValue>::CreateInstance(&pValue.m_p);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to create NullValue instance: 0x%X.", hr);
			AtlThrow(hr);
		}

		return pValue.Detach();
	}

	// Check value flags for how to dump.
	pLog->Append(L"Size of object is %u. Start getting object flags.", uValueSize);

	ULONG32 uValueFlags;
	hr = pClrValue->GetFlags(&uValueFlags);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get object flags: 0x%X.", hr);
		AtlThrow(hr);
	}

	pLog->Append(L"Object flags is 0x%X.", uValueFlags);

	if (uValueFlags & CLRDATA_VALUE_IS_REFERENCE)
	{
		pLog->Append(L"Dereferencing object.");
		CComPtr<IXCLRDataValue> pAssociatedValue;
		hr = pClrValue->GetAssociatedValue(&pAssociatedValue);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to dereferencing object: 0x%X.", hr);
			AtlThrow(hr);
		}
		
		return DumpClrDataValue(pAssociatedValue, pLog);
	}
	else if (uValueFlags & CLRDATA_VALUE_IS_ARRAY)
		return DumpClrArray(pClrValue, pLog);
	else if (uValueFlags & CLRDATA_VALUE_IS_STRING)
		return DumpClrString(pClrValue);
	else
		return DumpClrRawValue(pClrValue);
}

static CComPtr<IMethodParameter> DumpClrFrameArgument(IXCLRDataFrame *pClrFrame, ULONG32 uIndex, CExceptionDumpingMessageCollection *pLog)
{
	HRESULT hr;

	// Query Argument for specifies index.
	pLog->Append(L"Getting argument name and value.");

	CComPtr<IXCLRDataValue> pArgValue;
	WCHAR szArgName[128];
	hr = pClrFrame->GetArgumentByIndex(uIndex, &pArgValue, _countof(szArgName), NULL, szArgName);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get argument name and value: 0x%X.", hr);
		AtlThrow(hr);
	}

	// Construct object to hold dumped argument.
	pLog->Append(L"Creating MethodParameter instance for argument %s.", szArgName);

	CAutoPtr< CComObject<MethodParameter> > pParameter;
	hr = CComObject<MethodParameter>::CreateInstance(&pParameter.m_p);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to create MethodParameter instance: 0x%X.", hr);
		AtlThrow(hr);
	}

	pParameter->SetName(szArgName);

	// Query value's flags to determine type of argument.
	pLog->Append(L"Getting value flags.");

	ULONG32 uArgValueFlags;
	hr = pArgValue->GetFlags(&uArgValueFlags);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get value flags: 0x%X.", hr);
		AtlThrow(hr);
	}

	// Query argument's type.
	CComPtr<IXCLRDataTypeInstance> pArgType;
	if (uArgValueFlags & CLRDATA_VALUE_IS_REFERENCE)
	{
		pLog->Append(L"Getting value associated type.");
		hr = pArgValue->GetAssociatedType(&pArgType);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to get value associated type: 0x%X.", hr);
			AtlThrow(hr);
		}
	}
	else
	{
		pLog->Append(L"Getting value type.");
		hr = pArgValue->GetType(&pArgType);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to get value type: 0x%X.", hr);
			AtlThrow(hr);
		}
	}

	if (hr == S_OK)
	{
		pLog->Append(L"Getting value type name.");
		WCHAR szArgeTypeName[128];
		hr = pArgType->GetName(0, _countof(szArgeTypeName), NULL, szArgeTypeName);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to get value type name: 0x%X.", hr);
			AtlThrow(hr);
		}
		pParameter->SetType(szArgeTypeName);

		pLog->Append(L"Start dumping %s.", szArgeTypeName);
	}
	else
		pLog->Append(L"Start dumping value.");

	// Dump argument's value.
	CComPtr<IUnknown> pValue;
	try
	{
		pValue = DumpClrDataValue(pArgValue, pLog);
	}
	catch (CAtlException& e)
	{
		pLog->Append(L"Failed to dump argument's value: 0x%X.", e);
		pLog->Append(L"Creating UnknownValue.");

		CAutoPtr< CComObject<UnknownValue> > pUnknownValue;
		hr = CComObject<UnknownValue>::CreateInstance(&pUnknownValue.m_p);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to create UnknownValue: 0x%X.", hr);
			AtlThrow(hr);
		}

		pUnknownValue->AddRef();
		pValue = pUnknownValue.Detach();
	}

	pParameter->SetValue(pValue);

	return pParameter.Detach();
}

static CComPtr<IFrameData> DumpClrFrame(IXCLRDataFrame *pClrFrame, CExceptionDumpingMessageCollection *pLog)
{
	HRESULT hr;
	CAutoPtr< CComObject<FrameData> > pFrameData;

	// Construct object to hold dumped CLR frame.
	pLog->Append(L"Creating FrameData instance.");

	hr = CComObject<FrameData>::CreateInstance(&pFrameData.m_p);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to create FrameData instance: 0x%X.", hr);
		AtlThrow(hr);
	}

	// Dump Method.
	pLog->Append(L"Getting IXCLRDataMethodInstance.");

	CComPtr<IXCLRDataMethodInstance> pMethod;
	hr = pClrFrame->GetMethodInstance(&pMethod);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get IXCLRDataMethodInstance: 0x%X.", hr);
		AtlThrow(hr);
	}

	pLog->Append(L"Determine lenght of method name in the current frame.");
	ULONG32 uMethodNameLen;
	hr = pMethod->GetName(0, 0, &uMethodNameLen, NULL);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to determine length of method name in the current frame: 0x%X.", hr);
		AtlThrow(hr);
	}

	pLog->Append(L"Getting name of the method in the current frame.");
	CStringW strMethodName;
	hr = pMethod->GetName(0, uMethodNameLen, NULL, strMethodName.GetBufferSetLength(uMethodNameLen));
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get method name for current frame: 0x%X.", hr);
		AtlThrow(hr);
	}
	strMethodName.ReleaseBuffer();

	pFrameData->SetName(strMethodName);

	// Dump parameters.
	pLog->Append(L"Getting number of arguments for the method %s.", strMethodName);

	ULONG32 uArgs;
	hr = pClrFrame->GetNumArguments(&uArgs);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to get number of argument for the method: 0x%X.", hr);
		AtlThrow(hr);
	}

	pLog->Append(L"Start dumping %u arguments for the method.", uArgs);

	CComSafeArray<LPUNKNOWN> args(uArgs);

	for (ULONG32 i = 0; i < uArgs; i++)
	{
		pLog->Append(L"Dumping argument %u.", i);
		auto pArg = DumpClrFrameArgument(pClrFrame, i, pLog);
		hr = args.SetAt(i, pArg);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to save argument %u: 0x%X.", i, hr);
			AtlThrow(hr);
		}
	}

	pLog->Append(L"Finished dumping arguments of method %s. Total arguments: %u.", strMethodName, uArgs);
	pFrameData->SetParameters(args);

	return pFrameData.Detach();
}

static VOID DumpCurrentClrTask(DumpedData *pData, PEXCEPTION_POINTERS pException, CExceptionDumpingMessageCollection *pLog)
{
	CComCritSecLock<CComCriticalSection> locker(g_csClrDataProcess);
	HRESULT hr;
	CString strCurrentLoggerKey, strCurrentExceptionKey;

	// Setting thread-global data.
	pLog->Append(L"Setting logger for the current thread.");
	strCurrentLoggerKey.Format(TLSKEY_CURRENT_LOGGER, GetCurrentThreadId());
	g_pTls->Set(strCurrentLoggerKey, pLog);

	try
	{
		pLog->Append(L"Setting exception pointers for current thread.");
		strCurrentExceptionKey.Format(TLSKEY_CURRENT_EXCEPTION_POINTER, GetCurrentThreadId());
		g_pTls->Set(strCurrentExceptionKey, pException);
	}
	catch (...)
	{
		g_pTls->Remove(strCurrentLoggerKey);
		throw;
	}

	try
	{
		// Get CLR Task for the thread.
		pLog->Append(L"Getting IXCLRDataTask for current thread.");

		CComPtr<IXCLRDataTask> pClrTask;
		hr = g_pClrDataProcess->GetTaskByOSThreadID(GetCurrentThreadId(), &pClrTask);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to get IXCLRDataTask for current thread: 0x%X.", hr);
			AtlThrow(hr);
		}

		// Create stack walker to walk on CLR stack.
		pLog->Append(L"Creating IXCLRDataStackWalk.");

		CComPtr<IXCLRDataStackWalk> pClrStackWalk;
		hr = pClrTask->CreateStackWalk(CLRDATA_SIMPFRAME_MANAGED_METHOD | CLRDATA_SIMPFRAME_RUNTIME_MANAGED_CODE, &pClrStackWalk);
		if (FAILED(hr))
		{
			pLog->Append(L"Failed to create IXCLRDataStackWalk: 0x%X.", hr);
			AtlThrow(hr);
		}

		// Walk CLR stack.
		pLog->Append(L"Start iterating stack frame.");

		CInterfaceList<IFrameData> frames;
		UINT uMaxDumpedFrame = g_pConfig->GetMaxStackDepth();
		UINT i = 0;

		do
		{
			if (i >= uMaxDumpedFrame)
			{
				pLog->Append(L"Reached maximum number allowed frame.");
				break;
			}

			// Query current frame.
			pLog->Append(L"Getting frame %d.", i);

			CComPtr<IXCLRDataFrame> pClrFrame;
			hr = pClrStackWalk->GetFrame(&pClrFrame);
			if (FAILED(hr))
			{
				pLog->Append(L"Failed to get frame %d: 0x%X.", i, hr);
				AtlThrow(hr);
			}

			// Dump frame.
			pLog->Append(L"Start dumping frame %d.", i);

			auto pFrame = DumpClrFrame(pClrFrame, pLog);
			frames.AddTail(pFrame);

			// Moving to next frame.
			pLog->Append(L"Moving to next frame.");

			hr = pClrStackWalk->Next();
			if (FAILED(hr))
			{
				pLog->Append(L"Failed to move to next frame: 0x%X.", hr);
				AtlThrow(hr);
			}
			i++;
		} while (hr == S_OK);

		pLog->Append(L"Finished stack frames dumping. Total frames: %d.", i);
		pData->SetFrames(frames);
	}
	catch (...)
	{
		g_pClrDataProcess->Flush();
		g_pTls->Remove(strCurrentExceptionKey);
		g_pTls->Remove(strCurrentLoggerKey);
		throw;
	}

	g_pClrDataProcess->Flush();
	g_pTls->Remove(strCurrentExceptionKey);
	g_pTls->Remove(strCurrentLoggerKey);
}

static CComPtr<IDumpedData> DumpExceptionData(PEXCEPTION_POINTERS pException, CExceptionDumpingMessageCollection *pLog)
{
	HRESULT hr;

	pLog->Append(L"Begin dumping exception details.");

	// Construct object to hold exception data.
	CAutoPtr< CComObject<DumpedData> > pData;
	hr = CComObject<DumpedData>::CreateInstance(&pData.m_p);
	if (FAILED(hr))
	{
		pLog->Append(L"Failed to create DumpedData instance: 0x%X.", hr);
		AtlThrow(hr);
	}

	// Dump Data.
	DumpCurrentClrTask(pData, pException, pLog);

	return pData.Detach();
}

static VOID ClearLastExceptionLogs()
{
	if (!g_pTls->Exists(TLSKEY_LAST_EXCEPTION_DUMPING_LOG))
		return;
	g_pTls->Remove(TLSKEY_LAST_EXCEPTION_DUMPING_LOG);
}

static VOID ClearLastExceptionDump()
{
	ClearThreadLastExceptionData();
	ClearLastExceptionLogs();
}

static VOID HandleException(PEXCEPTION_POINTERS pException)
{
	// Create logging collection.
	CAutoPtr<CExceptionDumpingMessageCollection> pLog(new CExceptionDumpingMessageCollection());

	// Dump exception data.
	try
	{
		auto pData = DumpExceptionData(pException, pLog);
		g_pTls->Set(TLSKEY_LAST_EXCEPTION_DATA, pData);
	}
	catch (CAtlException& e)
	{
		pLog->Append(L"Failed to dump exception details: 0x%X.", e);
		ClearThreadLastExceptionData();
	}
	catch (...)
	{
		pLog->Append(L"Failed to dump exception details: Unknown Error.");
		ClearThreadLastExceptionData();
	}

	// Save logs.
	if (g_pConfig->GetLogExceptionDumping())
	{
		CAutoPtr< CComSafeArray<LPUNKNOWN> > pLogs(new CComSafeArray<LPUNKNOWN>());
		pLog->CopyTo(*pLogs);
		g_pTls->Set(TLSKEY_LAST_EXCEPTION_DUMPING_LOG, pLogs);
	}
	else if (g_pTls->Exists(TLSKEY_LAST_EXCEPTION_DUMPING_LOG))
		g_pTls->Remove(TLSKEY_LAST_EXCEPTION_DUMPING_LOG);
}

static LONG NTAPI VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	// Prevent recursive running. This make it safe to be able to throw exceptions in this function.
	// If we don't check for this, when exception occur it will call this function again and enter infinite loop until stack overflow.
	// FIXME: Use raw TlsGetValue/TlsSetValue instead to ensure no exception raising.
	if (g_pTls->Exists(TLSKEY_RUNNING_VEH))
		return EXCEPTION_CONTINUE_SEARCH;
	g_pTls->Set(TLSKEY_RUNNING_VEH, TRUE);

	try
	{
		if (g_pConfig->GetEnableExceptionDumper())
			HandleException(pExceptionInfo);
		else
			ClearLastExceptionDump();
	}
	catch (...)
	{
		ClearLastExceptionDump();
	}

	g_pTls->Remove(TLSKEY_RUNNING_VEH);
	return EXCEPTION_CONTINUE_SEARCH;
}

static PFN_CLRDataCreateInstance GetCLRDataCreateInstance()
{
	// Get location of C:\Windows.
	auto uBufSize = GetSystemWindowsDirectory(NULL, 0);
	if (!uBufSize)
		AtlThrowLastWin32();

	CPath dacPath;
	if (!GetSystemWindowsDirectory(dacPath.m_strPath.GetBufferSetLength(uBufSize), uBufSize))
		AtlThrowLastWin32();
	dacPath.m_strPath.ReleaseBuffer();

	// Construct full path of "C:\Windows\Microsoft.NET\Framework64\v4.0.30319\mscordacwks.dll".
	if (!dacPath.Append(CLR_DIRECTORY))
		AtlThrow(E_FAIL);
	if (!dacPath.Append(_T("mscordacwks.dll")))
		AtlThrow(E_FAIL);

	// Load mscordacwks.dll if it not loaded.
	auto hModule = GetModuleHandle(dacPath);
	if (!hModule)
	{
		auto dwError = GetLastError();
		if (dwError != ERROR_MOD_NOT_FOUND)
			AtlThrow(HRESULT_FROM_WIN32(dwError));

		hModule = LoadLibrary(dacPath);
		if (!hModule)
			AtlThrowLastWin32();
	}

	// Get a pointer to CLRDataCreateInstance().
	auto pCLRDataCreateInstance = reinterpret_cast<PFN_CLRDataCreateInstance>(GetProcAddress(hModule, "CLRDataCreateInstance"));
	if (!pCLRDataCreateInstance)
		AtlThrowLastWin32();

	return pCLRDataCreateInstance;
}

VOID InitialiseVectoredExceptionHandler()
{
	HRESULT hr;

	ATLTRACE2(_T("[%u] InitialiseVectoredExceptionHandler()"), GetCurrentThreadId());

	CComPtr<CComObject<ClrDataTarget>> pClrDataTarget;
	hr = CComObject<ClrDataTarget>::CreateInstance(&pClrDataTarget);
	if (FAILED(hr))
		AtlThrow(hr);
	pClrDataTarget.p->AddRef();

	hr = g_csClrDataProcess.Init();
	if (FAILED(hr))
		AtlThrow(hr);

	hr = GetCLRDataCreateInstance()(__uuidof(IXCLRDataProcess), pClrDataTarget, reinterpret_cast<void **>(&g_pClrDataProcess));
	if (FAILED(hr))
		AtlThrow(hr);

	try
	{
		g_pVehHandle = AddVectoredExceptionHandler(FALSE, VectoredExceptionHandler);
		if (!g_pVehHandle)
			AtlThrow(E_FAIL);
	}
	catch (...)
	{
		g_pClrDataProcess->Release();
		g_pClrDataProcess = NULL;
		throw;
	}
}

BOOL FinaliseVectoredExceptionHandler()
{
	if (g_pVehHandle && !RemoveVectoredExceptionHandler(g_pVehHandle))
		return FALSE;

	if (g_pClrDataProcess)
		g_pClrDataProcess->Release();

	return TRUE;
}

CComPtr<IDumpedData> GetThreadLastExceptionData()
{
	auto pTlsVal = static_cast<CInterfaceTlsValue<IDumpedData> *>(g_pTls->Get(TLSKEY_LAST_EXCEPTION_DATA));
	if (!pTlsVal)
		return NULL;
	return pTlsVal->GetValue();
}

VOID ClearThreadLastExceptionData()
{
	if (!g_pTls->Exists(TLSKEY_LAST_EXCEPTION_DATA))
		return;
	g_pTls->Remove(TLSKEY_LAST_EXCEPTION_DATA);
}
