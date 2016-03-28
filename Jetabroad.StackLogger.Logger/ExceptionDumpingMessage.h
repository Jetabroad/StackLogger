#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// ExceptionDumpingMessage contain logging message for exception dumping operation.
// It expose directly to managed world via IExceptionDumpingMessage.
class ATL_NO_VTABLE ExceptionDumpingMessage :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IExceptionDumpingMessage
{
public:
	BEGIN_COM_MAP(ExceptionDumpingMessage)
		COM_INTERFACE_ENTRY(IExceptionDumpingMessage)
	END_COM_MAP()
public:
	ExceptionDumpingMessage(const ExceptionDumpingMessage&) = delete;
	ExceptionDumpingMessage& operator=(const ExceptionDumpingMessage&) = delete;

	VOID SetTime(DATE time);
	VOID SetMessage(PCWSTR pszMessage);
	
	virtual HRESULT STDMETHODCALLTYPE get_Time(DATE *time);
	virtual HRESULT STDMETHODCALLTYPE get_Message(BSTR *msg);
protected:
	ExceptionDumpingMessage();
	virtual ~ExceptionDumpingMessage();
private:
	COleDateTime m_time;
	CComBSTR m_strMsg;
};
