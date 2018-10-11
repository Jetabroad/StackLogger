// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// RawValue used for store raw managed object in case we don't known how to dump it.
// It expose directly to managed world via IRawValue.
class ATL_NO_VTABLE RawValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<RawValue>,
	public IRawValue
{
public:
	RawValue(const RawValue&) = delete;
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(RawValue)
		COM_INTERFACE_ENTRY(IRawValue)
	END_COM_MAP()
public:
	RawValue& operator=(const RawValue&) = delete;

	VOID SetValue(const CComSafeArray<BYTE>& value);
public:
	HRESULT STDMETHODCALLTYPE get_Value(SAFEARRAY **value) override;
protected:
	RawValue();
	~RawValue();
private:
	CComSafeArray<BYTE> val;
};
