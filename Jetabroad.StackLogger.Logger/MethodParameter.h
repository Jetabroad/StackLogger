#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// MethodParameter contains information about argument for managed method.
// It expose directly to managed world via IMethodParameter.
class ATL_NO_VTABLE MethodParameter :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IMethodParameter
{
public:
	BEGIN_COM_MAP(MethodParameter)
		COM_INTERFACE_ENTRY(IMethodParameter)
	END_COM_MAP()
public:
	MethodParameter(const MethodParameter&) = delete;
	MethodParameter& operator=(const MethodParameter&) = delete;

	VOID SetName(PCWSTR pszName);
	VOID SetType(PCWSTR pszName);
	VOID SetValue(const CComPtr<IUnknown>& pValue);
	
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR *name);
	virtual HRESULT STDMETHODCALLTYPE get_Type(BSTR *name);
	virtual HRESULT STDMETHODCALLTYPE get_Value(IUnknown **value);
protected:
	MethodParameter();
	virtual ~MethodParameter();
private:
	CComBSTR m_strName;
	CComBSTR m_strType;
	CComPtr<IUnknown> m_pValue;
};
