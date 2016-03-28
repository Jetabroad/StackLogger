#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// DumpedData used for store information about managed exception.
// It expose directly to managed world via IDumpedData.
class ATL_NO_VTABLE DumpedData :
	public CComObjectRootEx<CComObjectThreadModel>,
	public IDumpedData
{
public:
	BEGIN_COM_MAP(DumpedData)
		COM_INTERFACE_ENTRY(IDumpedData)
	END_COM_MAP()
public:
	DumpedData(const DumpedData&) = delete;
	DumpedData& operator=(const DumpedData&) = delete;

	VOID SetFrames(const CInterfaceList<IFrameData>& frames);
	
	virtual HRESULT STDMETHODCALLTYPE get_Frames(SAFEARRAY **frames);
protected:
	DumpedData();
	~DumpedData();
private:
	CComSafeArray<LPUNKNOWN> m_frames;
};
