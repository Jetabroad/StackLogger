#include "PCH.h"
#include "ObjectDescriptor2.h"

ObjectDescriptor2::ObjectDescriptor2()
{
}

ObjectDescriptor2::~ObjectDescriptor2()
{
}

VOID ObjectDescriptor2::SetName(PCWSTR pszName)
{
	name = pszName;
}

VOID ObjectDescriptor2::SetType(PCWSTR pszName)
{
	type = pszName;
}

VOID ObjectDescriptor2::SetValue(const CComPtr<IUnknown>& pValue)
{
	val = pValue;
}

HRESULT ObjectDescriptor2::get_Type(BSTR *name)
{
	return type.CopyTo(name);
}

HRESULT ObjectDescriptor2::get_Value(IUnknown **value)
{
	return val.CopyTo(value);
}

HRESULT ObjectDescriptor2::get_Name(BSTR *name)
{
	return this->name.CopyTo(name);
}
