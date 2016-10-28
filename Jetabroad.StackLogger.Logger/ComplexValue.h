#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

class ATL_NO_VTABLE ComplexValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<ComplexValue>,
	public IComplexValue
{
public:
	ComplexValue(const ComplexValue&) = delete;
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(ComplexValue)
		COM_INTERFACE_ENTRY(IComplexValue)
	END_COM_MAP()
public:
	ComplexValue& operator=(const ComplexValue&) = delete;

	VOID SetFields(const CInterfaceArray<IObjectDescriptor2>& fields);
public:
	HRESULT STDMETHODCALLTYPE get_Fields(SAFEARRAY **fields) override;
protected:
	ComplexValue();
	~ComplexValue();
private:
	CComSafeArray<LPUNKNOWN> fields;
};
