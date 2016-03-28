#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// NullValue represents the managed null object.
// It expose directly to managed world via INullValue.
class ATL_NO_VTABLE NullValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public INullValue
{
public:
	BEGIN_COM_MAP(NullValue)
		COM_INTERFACE_ENTRY(INullValue)
	END_COM_MAP()
public:
	NullValue(const NullValue&) = delete;
	NullValue& operator=(const NullValue&) = delete;
protected:
	NullValue();
	virtual ~NullValue();
};
