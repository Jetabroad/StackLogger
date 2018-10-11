// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// ArrayValue is used to store dumped managed array.
// It is expose directly to managed world via IArrayValue.
class ATL_NO_VTABLE ArrayValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<ArrayValue>,
	public IArrayValue
{
public:
	ArrayValue(const ArrayValue&) = delete;
public:
	BEGIN_COM_MAP(ArrayValue)
		COM_INTERFACE_ENTRY(IArrayValue)
	END_COM_MAP()
public:
	ArrayValue& operator=(const ArrayValue&) = delete;

	VOID SetValues(const CComSafeArray<LPUNKNOWN>& values);
public:
	HRESULT STDMETHODCALLTYPE get_Values(SAFEARRAY **items) override;
protected:
	ArrayValue();
	~ArrayValue();
private:
	CComSafeArray<LPUNKNOWN> vals;
};
