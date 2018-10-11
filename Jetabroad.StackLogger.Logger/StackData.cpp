// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
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
