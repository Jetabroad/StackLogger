#include "PCH.h"
#include "FrameData.h"

FrameData::FrameData() : m_strName(L"")
{
}

FrameData::~FrameData()
{
}

VOID FrameData::SetParameters(const CComSafeArray<LPUNKNOWN>& params)
{
	m_params = params;
}

VOID FrameData::SetName(PCWSTR pszName)
{
	if (!pszName)
		AtlThrow(E_POINTER);
	m_strName = pszName;
}

HRESULT FrameData::get_Parameters(SAFEARRAY **params)
{
	return m_params.CopyTo(params);
}

HRESULT FrameData::get_Name(BSTR *name)
{
	return m_strName.CopyTo(name);
}
