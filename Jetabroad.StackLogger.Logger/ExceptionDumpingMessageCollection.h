#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// Collection for ExceptionDumpingMessage.
class CExceptionDumpingMessageCollection final
{
public:
	CExceptionDumpingMessageCollection();
	CExceptionDumpingMessageCollection(const CExceptionDumpingMessageCollection&) = delete;
	~CExceptionDumpingMessageCollection();

	CExceptionDumpingMessageCollection& operator=(const CExceptionDumpingMessageCollection&) = delete;

	VOID Append(PCWSTR pszFormat, ...);
	VOID CopyTo(CComSafeArray<LPUNKNOWN>& target) const;
private:
	CInterfaceList<IExceptionDumpingMessage> m_messages;
};
