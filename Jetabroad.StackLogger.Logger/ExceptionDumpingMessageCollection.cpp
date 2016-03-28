#include "PCH.h"
#include "ExceptionDumpingMessageCollection.h"
#include "ExceptionDumpingMessage.h"
#include "GlobalConfiguration.h"

static CComPtr<IExceptionDumpingMessage> CreateMessage(PCWSTR pszMessage, const COleDateTime& time)
{
	HRESULT hr;

	CAutoPtr< CComObject<ExceptionDumpingMessage> > pMessage;
	hr = CComObject<ExceptionDumpingMessage>::CreateInstance(&pMessage.m_p);
	if (FAILED(hr))
		AtlThrow(hr);

	pMessage->SetMessage(pszMessage);
	pMessage->SetTime(time);

	return pMessage.Detach();
}

static CComPtr<IExceptionDumpingMessage> CreateMessage(PCWSTR pszMessage)
{
	SYSTEMTIME now;
	GetLocalTime(&now);

	return CreateMessage(pszMessage, now);
}

CExceptionDumpingMessageCollection::CExceptionDumpingMessageCollection()
{
}

CExceptionDumpingMessageCollection::~CExceptionDumpingMessageCollection()
{
}

VOID CExceptionDumpingMessageCollection::Append(PCWSTR pszFormat, ...)
{
	if (!g_pConfig->GetLogExceptionDumping())
		return;

	va_list args;

	CStringW strMessage;
	va_start(args, pszFormat);
	strMessage.FormatV(pszFormat, args);
	va_end(args);

	auto pMessage = CreateMessage(strMessage);
	m_messages.AddTail(pMessage);
}

VOID CExceptionDumpingMessageCollection::CopyTo(CComSafeArray<LPUNKNOWN>& target) const
{
	HRESULT hr;

	hr = target.Destroy();
	if (FAILED(hr))
		AtlThrow(hr);

	hr = target.Create(static_cast<ULONG>(m_messages.GetCount()));
	if (FAILED(hr))
		AtlThrow(hr);

	auto pPos = m_messages.GetHeadPosition();
	for (LONG i = 0; pPos; i++)
	{
		auto pMessage = m_messages.GetNext(pPos);
		hr = target.SetAt(i, pMessage);
		if (FAILED(hr))
			AtlThrow(hr);
	}
}
