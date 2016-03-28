#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// StringValue contains managed string. It expose directly to managed world via IStringValue.
class ATL_NO_VTABLE StringValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IStringValue
{
public:
	BEGIN_COM_MAP(StringValue)
		COM_INTERFACE_ENTRY(IStringValue)
	END_COM_MAP()
public:
	StringValue(const StringValue&) = delete;
	StringValue& operator=(const StringValue&) = delete;

	VOID SetValue(PCWSTR pszValue);
	
	virtual HRESULT STDMETHODCALLTYPE get_Value(BSTR *value);
protected:
	StringValue();
	virtual ~StringValue();
private:
	CComBSTR m_strValue;
};
