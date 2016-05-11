#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// ObjectDescriptor2 contains information about object that have a name (method parameter, etc.).
// It expose directly to managed world via IObjectDescriptor2.
class ATL_NO_VTABLE ObjectDescriptor2 :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<ObjectDescriptor2>,
	public IObjectDescriptor2
{
public:
	ObjectDescriptor2(const ObjectDescriptor2&) = delete;
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(ObjectDescriptor2)
		COM_INTERFACE_ENTRY(IObjectDescriptor)
		COM_INTERFACE_ENTRY(IObjectDescriptor2)
	END_COM_MAP()
public:
	ObjectDescriptor2& operator=(const ObjectDescriptor2&) = delete;

	VOID SetName(PCWSTR pszName);
	VOID SetType(PCWSTR pszName);
	VOID SetValue(const CComPtr<IUnknown>& pValue);
public:
	HRESULT STDMETHODCALLTYPE get_Type(BSTR *name) override;
	HRESULT STDMETHODCALLTYPE get_Value(IUnknown **value) override;
public:
	HRESULT STDMETHODCALLTYPE get_Name(BSTR *name) override;
protected:
	ObjectDescriptor2();
	~ObjectDescriptor2();
private:
	CComBSTR name;
	CComBSTR type;
	CComPtr<IUnknown> val;
};
