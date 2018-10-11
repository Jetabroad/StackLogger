// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// This class used for store call stack information.
// It expose directly to managed world via IStackData.
class ATL_NO_VTABLE StackData :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<StackData>,
	public IStackData
{
public:
	BEGIN_COM_MAP(StackData)
		COM_INTERFACE_ENTRY(IStackData)
	END_COM_MAP()
public:
	StackData(const StackData&) = delete;
	StackData& operator=(const StackData&) = delete;

	VOID SetFrames(const CInterfaceArray<IFrameData>& frames);
	
	HRESULT STDMETHODCALLTYPE get_Frames(SAFEARRAY **frames) override;
protected:
	StackData();
private:
	CComSafeArray<LPUNKNOWN> frames;
};
