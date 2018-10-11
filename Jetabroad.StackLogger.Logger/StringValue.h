// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
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
