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
