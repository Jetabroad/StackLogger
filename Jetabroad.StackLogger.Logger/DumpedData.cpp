#include "PCH.h"
#include "DumpedData.h"

DumpedData::DumpedData()
{
}

DumpedData::~DumpedData()
{
}

VOID DumpedData::SetFrames(const CInterfaceList<IFrameData>& frames)
{
	HRESULT hr;

	hr = m_frames.Create(static_cast<ULONG>(frames.GetCount()));
	if (FAILED(hr))
		AtlThrow(hr);

	POSITION pPos;
	LONG i;

	for (i = 0, pPos = frames.GetHeadPosition(); pPos; i++)
	{
		auto pFrame = frames.GetNext(pPos);
		hr = m_frames.SetAt(i, pFrame);
		if (FAILED(hr))
			AtlThrow(hr);
	}
}

HRESULT DumpedData::get_Frames(SAFEARRAY **frames)
{
	return m_frames.CopyTo(frames);
}
