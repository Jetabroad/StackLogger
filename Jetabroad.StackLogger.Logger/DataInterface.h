#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

// DataInterface is interface for managed world to retrieve data from unmanaged world.
// It is expose to managed world directly via IDataInterface.
class ATL_NO_VTABLE DataInterface :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<DataInterface, &CLSID_DataInterface>,
	public IDataInterface
{
public:
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(DataInterface)
		COM_INTERFACE_ENTRY(IDataInterface)
	END_COM_MAP()
public:
	DataInterface(const DataInterface&) = delete;
	DataInterface& operator=(const DataInterface&) = delete;

	HRESULT FinalConstruct();

	static VOID Init();
	static BOOL Term();

	virtual HRESULT STDMETHODCALLTYPE get_HasLastDumped(VARIANT_BOOL *result);
	virtual HRESULT STDMETHODCALLTYPE get_HasLastExceptionDumpingLogs(VARIANT_BOOL *result);
	virtual HRESULT STDMETHODCALLTYPE ClearLastDumped();
	virtual HRESULT STDMETHODCALLTYPE QueryLastDumped(IDumpedData **data);
	virtual HRESULT STDMETHODCALLTYPE QueryLastExceptionDumpingLogs(SAFEARRAY **logs);
protected:
	DataInterface();
	virtual ~DataInterface();
};
