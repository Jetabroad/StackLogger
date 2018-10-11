// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#include "PCH.h"
#include "StringValue.h"

StringValue::StringValue()
{
}

StringValue::~StringValue()
{
}

VOID StringValue::SetValue(PCWSTR pszValue)
{
	val = pszValue;
}

HRESULT StringValue::get_Value(BSTR *value)
{
	return val.CopyTo(value);
}
