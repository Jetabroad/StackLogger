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
