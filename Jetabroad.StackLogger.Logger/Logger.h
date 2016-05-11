#pragma once

class ATL_NO_VTABLE CLogger
{
public:
	virtual VOID WriteLog(LPCWSTR pszFormat, ...) noexcept = 0;
protected:
	CLogger();
	virtual ~CLogger();
};
