// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// FrameData contains information about managed frame.
// It is expose directly to managed world via IFrameData.
class ATL_NO_VTABLE FrameData :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IFrameData
{
public:
	BEGIN_COM_MAP(FrameData)
		COM_INTERFACE_ENTRY(IFrameData)
	END_COM_MAP()
public:
	FrameData(const FrameData&) = delete;
	FrameData& operator=(const FrameData&) = delete;

	VOID SetParameters(const CComSafeArray<LPUNKNOWN>& params);
	VOID SetName(PCWSTR pszName);
	
	virtual HRESULT STDMETHODCALLTYPE get_Parameters(SAFEARRAY **params);
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR *name);
protected:
	FrameData();
	virtual ~FrameData();
private:
	CComSafeArray<LPUNKNOWN> m_params;
	CComBSTR m_strName;
};
