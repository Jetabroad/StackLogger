#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// RawValue used for store raw managed object in case we don't known how to dump it.
// It expose directly to managed world via IRawValue.
class ATL_NO_VTABLE RawValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IRawValue
{
public:
	BEGIN_COM_MAP(RawValue)
		COM_INTERFACE_ENTRY(IRawValue)
	END_COM_MAP()
public:
	RawValue(const RawValue&) = delete;
	RawValue& operator=(const RawValue&) = delete;

	VOID SetValue(const CComPtr<IStream>& pValue);
	
	virtual HRESULT STDMETHODCALLTYPE get_Value(IStream **value);
protected:
	RawValue();
	virtual ~RawValue();
private:
	CComPtr<IStream> m_pValue;
};
