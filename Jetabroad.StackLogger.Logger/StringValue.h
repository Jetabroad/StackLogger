#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// StringValue contains managed string. It expose directly to managed world via IStringValue.
class ATL_NO_VTABLE StringValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<StringValue>,
	public IStringValue
{
public:
	StringValue(const StringValue&) = delete;
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(StringValue)
		COM_INTERFACE_ENTRY(IStringValue)
	END_COM_MAP()
public:
	StringValue& operator=(const StringValue&) = delete;

	VOID SetValue(PCWSTR pszValue);
public:
	HRESULT STDMETHODCALLTYPE get_Value(BSTR *value) override;
protected:
	StringValue();
	~StringValue();
private:
	CComBSTR val;
};
