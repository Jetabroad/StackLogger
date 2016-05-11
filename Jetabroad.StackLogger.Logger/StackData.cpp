#include "PCH.h"
#include "StackData.h"

StackData::StackData()
{
}

VOID StackData::SetFrames(const CInterfaceArray<IFrameData>& frames)
{
	LONG nframes = static_cast<LONG>(frames.GetCount());

	auto hr = this->frames.Create(nframes);
	if (FAILED(hr))
		AtlThrow(hr);

	for (LONG i = 0; i < nframes; i++)
	{
		hr = this->frames.SetAt(i, frames[i]);
		if (FAILED(hr))
			AtlThrow(hr);
	}
}

HRESULT StackData::get_Frames(SAFEARRAY **frames)
{
	return this->frames.CopyTo(frames);
}
