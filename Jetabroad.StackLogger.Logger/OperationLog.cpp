#include "PCH.h"
#include "OperationLog.h"

OperationLog::OperationLog() :
	time(_time64(NULL)),
	msg(L"")
{
}

VOID OperationLog::SetTime(DATE time)
{
	this->time = time;
}

VOID OperationLog::SetMessage(PCWSTR pszMessage)
{
	if (!pszMessage)
		AtlThrow(E_POINTER);

	msg = pszMessage;
}

HRESULT OperationLog::get_Time(DATE *time)
{
	if (!time)
		return E_POINTER;

	*time = this->time;

	return S_OK;
}

HRESULT OperationLog::get_Message(BSTR *msg)
{
	return this->msg.CopyTo(msg);
}
