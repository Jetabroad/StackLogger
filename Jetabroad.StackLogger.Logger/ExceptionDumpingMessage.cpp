#include "PCH.h"
#include "ExceptionDumpingMessage.h"

ExceptionDumpingMessage::ExceptionDumpingMessage() : m_time(_time64(NULL)), m_strMsg(L"")
{
}

ExceptionDumpingMessage::~ExceptionDumpingMessage()
{
}

VOID ExceptionDumpingMessage::SetTime(DATE time)
{
	m_time = time;
}

VOID ExceptionDumpingMessage::SetMessage(PCWSTR pszMessage)
{
	if (!pszMessage)
		AtlThrow(E_POINTER);
	m_strMsg = pszMessage;
}

HRESULT ExceptionDumpingMessage::get_Time(DATE *time)
{
	if (!time) return E_POINTER;
	*time = m_time;
	return S_OK;
}

HRESULT ExceptionDumpingMessage::get_Message(BSTR *msg)
{
	return m_strMsg.CopyTo(msg);
}
