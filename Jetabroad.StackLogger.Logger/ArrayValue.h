#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// ArrayValue is used to store dumped managed array.
// It is expose directly to managed world via IArrayValue.
class ATL_NO_VTABLE ArrayValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IArrayValue
{
public:
	BEGIN_COM_MAP(ArrayValue)
		COM_INTERFACE_ENTRY(IArrayValue)
	END_COM_MAP()
public:
	ArrayValue(const ArrayValue&) = delete;
	ArrayValue& operator=(const ArrayValue&) = delete;

	VOID SetValues(const CComSafeArray<LPUNKNOWN>& values);
	
	virtual HRESULT STDMETHODCALLTYPE get_Values(SAFEARRAY **items);
protected:
	ArrayValue();
	virtual ~ArrayValue();
private:
	CComSafeArray<LPUNKNOWN> m_values;
};
