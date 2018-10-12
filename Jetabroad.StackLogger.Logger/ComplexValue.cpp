#include "PCH.h"
#include "ComplexValue.h"

ComplexValue::ComplexValue()
{
}

ComplexValue::~ComplexValue()
{
}

VOID ComplexValue::SetFields(const CInterfaceArray<IObjectDescriptor2>& fields)
{
	auto nf = static_cast<LONG>(fields.GetCount());
	auto hr = this->fields.Create(nf);

	if (FAILED(hr))
		AtlThrow(hr);

	for (LONG i = 0; i < nf; i++)
	{
		hr = this->fields.SetAt(i, fields[i]);
		if (FAILED(hr))
			AtlThrow(hr);
	}
}

HRESULT ComplexValue::get_Fields(SAFEARRAY **fields)
{
	return this->fields.CopyTo(fields);
}
