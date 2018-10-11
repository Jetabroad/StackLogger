// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#include "PCH.h"
#include "ArrayValue.h"

ArrayValue::ArrayValue()
{
}

ArrayValue::~ArrayValue()
{
}

VOID ArrayValue::SetValues(const CComSafeArray<LPUNKNOWN>& values)
{
	vals = values;
}

HRESULT ArrayValue::get_Values(SAFEARRAY **items)
{
	return vals.CopyTo(items);
}
