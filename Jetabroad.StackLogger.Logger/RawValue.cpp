#include "PCH.h"
#include "RawValue.h"

RawValue::RawValue()
{
}

RawValue::~RawValue()
{
}

VOID RawValue::SetValue(const CComPtr<IStream>& pValue)
{
	m_pValue = pValue;
}

HRESULT RawValue::get_Value(IStream **value)
{
	return m_pValue.CopyTo(value);
}
