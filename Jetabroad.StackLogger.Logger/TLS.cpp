#include "PCH.h"
#include "TLS.h"

CTLS *g_pTls;

CTLS::CTLS()
{
	m_dwTlsIndex = TlsAlloc();
	if (m_dwTlsIndex == TLS_OUT_OF_INDEXES)
		AtlThrowLastWin32();
}

CTLS::~CTLS()
{
	TlsFree(m_dwTlsIndex);

	for (auto pBucketIter = m_buckets.GetStartPosition(); pBucketIter; )
		delete m_buckets.GetNextKey(pBucketIter);
}

VOID CTLS::DestroyBucket()
{
	auto pBucket = reinterpret_cast<CValueBucket *>(TlsGetValue(m_dwTlsIndex));
	if (!pBucket)
		return;

	TlsSetValue(m_dwTlsIndex, NULL);

	try
	{
		{
			CComCritSecLock<CComAutoCriticalSection> locker(m_csBuckets);
			m_buckets.RemoveKey(pBucket);
		}
		delete pBucket;
	}
	catch (...)
	{
		// Swallow.
	}
}

BOOL CTLS::Exists(const CString& strKey) const
{
	auto pBucket = reinterpret_cast<const CValueBucket *>(TlsGetValue(m_dwTlsIndex));
	if (!pBucket)
		return FALSE;

	return pBucket->Lookup(strKey) != NULL;
}

CTlsValue * CTLS::Get(const CString& strKey) const
{
	auto pBucket = reinterpret_cast<const CValueBucket *>(TlsGetValue(m_dwTlsIndex));
	if (!pBucket)
		return NULL;

	auto pPair = pBucket->Lookup(strKey);
	if (!pPair)
		return NULL;

	return pPair->m_value;
}

CAutoPtr<CTlsValue> CTLS::Remove(const CString& strKey)
{
	auto pBucket = reinterpret_cast<CValueBucket *>(TlsGetValue(m_dwTlsIndex));
	if (!pBucket)
		AtlThrow(E_FAIL);

	auto pPair = pBucket->Lookup(strKey);
	if (!pPair)
		AtlThrow(E_FAIL);

	auto pVal = pPair->m_value;
	pBucket->RemoveKey(strKey);

	return pVal;
}

VOID CTLS::Set(const CString& strKey, CAutoPtr<CTlsValue>& pValue)
{
	auto pBucket = reinterpret_cast<CValueBucket *>(TlsGetValue(m_dwTlsIndex));
	if (!pBucket)
	{
		pBucket = new CValueBucket();

		try
		{
			CComCritSecLock<CComAutoCriticalSection> locker(m_csBuckets);
			m_buckets.SetAt(pBucket, TRUE);
		}
		catch (...)
		{
			delete pBucket;
			throw;
		}

		TlsSetValue(m_dwTlsIndex, pBucket);
	}

	pBucket->SetAt(strKey, pValue);
}
