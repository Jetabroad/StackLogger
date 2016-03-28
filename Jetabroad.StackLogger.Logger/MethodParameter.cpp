#include "PCH.h"
#include "MethodParameter.h"

MethodParameter::MethodParameter() : m_strName(L""), m_strType(L"")
{
}

MethodParameter::~MethodParameter()
{
}

VOID MethodParameter::SetName(PCWSTR pszName)
{
	if (!pszName)
		AtlThrow(E_POINTER);
	m_strName = pszName;
}

VOID MethodParameter::SetType(PCWSTR pszName)
{
	if (!pszName)
		AtlThrow(E_POINTER);
	m_strType = pszName;
}

VOID MethodParameter::SetValue(const CComPtr<IUnknown>& pValue)
{
	m_pValue = pValue;
}

HRESULT MethodParameter::get_Name(BSTR *name)
{
	return m_strName.CopyTo(name);
}

HRESULT MethodParameter::get_Type(BSTR *name)
{
	return m_strType.CopyTo(name);
}

HRESULT MethodParameter::get_Value(IUnknown **value)
{
	return m_pValue.CopyTo(value);
}
