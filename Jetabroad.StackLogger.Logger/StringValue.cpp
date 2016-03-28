#include "PCH.h"
#include "StringValue.h"

StringValue::StringValue() : m_strValue("")
{
}

StringValue::~StringValue()
{
}

VOID StringValue::SetValue(PCWSTR pszValue)
{
	if (!pszValue)
		AtlThrow(E_POINTER);
	m_strValue = pszValue;
}

HRESULT StringValue::get_Value(BSTR *value)
{
	return m_strValue.CopyTo(value);
}
