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
	m_values = values;
}

HRESULT ArrayValue::get_Values(SAFEARRAY **items)
{
	if (!m_values)
		return E_NOTIMPL;
	return m_values.CopyTo(items);
}
