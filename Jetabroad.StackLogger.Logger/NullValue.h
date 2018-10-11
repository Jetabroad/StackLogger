// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// NullValue represents the managed null object.
// It expose directly to managed world via INullValue.
class ATL_NO_VTABLE NullValue :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<NullValue>,
	public INullValue
{
public:
	NullValue(const NullValue&) = delete;
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(NullValue)
		COM_INTERFACE_ENTRY(INullValue)
	END_COM_MAP()
public:
	NullValue& operator=(const NullValue&) = delete;
protected:
	NullValue();
	~NullValue();
};
