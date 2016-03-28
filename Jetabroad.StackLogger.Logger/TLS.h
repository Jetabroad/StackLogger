#pragma once

#include "TlsValue.h"

#define TLSKEY_LAST_EXCEPTION_DATA _T("LastExceptionData")
#define TLSKEY_LAST_EXCEPTION_DUMPING_LOG _T("LastExceptionDumpingLog")
#define TLSKEY_RUNNING_VEH _T("RunningVEH")
#define TLSKEY_CURRENT_EXCEPTION_POINTER _T("CurrentExceptionPointer - %u")
#define TLSKEY_CURRENT_LOGGER _T("CurrentLogger - %u")

// Thread local storage API wrapper for automatic memory management.
// Use to store/retrieve data for the calling thread.
// Stored data will not opaque to other threads.
class CTLS
{
	typedef CMapToAutoPtr<CString, CTlsValue, CStringRefElementTraits<CString>> CValueBucket;
public:
	CTLS();
	CTLS(const CTLS&) = delete;
	~CTLS();
public:
	CTLS& operator=(const CTLS&) = delete;

	VOID DestroyBucket();
	BOOL Exists(const CString& strKey) const;
	CTlsValue * Get(const CString& strKey) const;
	CAutoPtr<CTlsValue> Remove(const CString& strKey);
	VOID Set(const CString& strKey, CAutoPtr<CTlsValue>& pValue);

	template<typename T>
	VOID Set(const CString& strKey, const T& val)
	{
		CAutoPtr<CTlsValue> pValue(new CGeneralTlsValue<T>(val));
		Set(strKey, pValue);
	}

	template<typename T>
	VOID Set(const CString& strKey, CAutoPtr<T>& val)
	{
		CAutoPtr<CTlsValue> pValue(new CAutoPtrTlsValue<T>(val));
		val.Detach(); // Ownership is already transfered to pValue.
		Set(strKey, pValue);
	}
	
	// Force link error to prevent bug since we don't allow "const CAutoPtr<T>" to go to "Set(const CString& strKey, const T& val)".
	template<typename T>
	VOID Set(const CString& strKey, const CAutoPtr<T>& val);

	template<typename T>
	VOID Set(const CString& strKey, const CComPtr<T>& val)
	{
		CAutoPtr<CTlsValue> pValue(new CInterfaceTlsValue<T>(val));
		Set(strKey, pValue);
	}
private:
	DWORD m_dwTlsIndex;
	CAtlMap<CValueBucket *, BOOL> m_buckets;
	CComAutoCriticalSection m_csBuckets;
};

extern CTLS *g_pTls;
