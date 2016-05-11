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
