// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#include "PCH.h"
#include "RawValue.h"

RawValue::RawValue()
{
}

RawValue::~RawValue()
{
}

VOID RawValue::SetValue(const CComSafeArray<BYTE>& value)
{
	val = value;
}

HRESULT RawValue::get_Value(SAFEARRAY **value)
{
	return val.CopyTo(value);
}
