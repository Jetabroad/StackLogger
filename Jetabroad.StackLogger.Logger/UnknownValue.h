// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// UnknownValue represents the managed object that fail to dump.
// It expose directly to managed world via IUnknownValue.
class ATL_NO_VTABLE UnknownValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<UnknownValue>,
	public IUnknownValue
{
public:
	UnknownValue(const UnknownValue&) = delete;
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(UnknownValue)
		COM_INTERFACE_ENTRY(IUnknownValue)
	END_COM_MAP()
public:
	UnknownValue& operator=(const UnknownValue&) = delete;
protected:
	UnknownValue();
	~UnknownValue();
};
