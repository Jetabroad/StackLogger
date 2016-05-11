#pragma once

#include "CallbackThunk.h"
#include "ExceptionDumper.h"
#include "Logger.h"
#include "ModuleHandle.h"
#include "ThreadLocalStorage.h"

#include "Jetabroad.StackLogger.Logger_h.h"

// Interface to unmanaged world for managed world.
class ATL_NO_VTABLE StackLogger :
	public CComObjectRootEx<CComObjectThreadModel>,
	public CComCoClass<StackLogger, &CLSID_StackLogger>,
	public IStackLogger,
	private CLogger
{
public: // Public constructors and destructor.
	StackLogger(const StackLogger&) = delete;
public: // ATL & WTL Attributes.
	DECLARE_NO_REGISTRY()
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	BEGIN_COM_MAP(StackLogger)
		COM_INTERFACE_ENTRY(IStackLogger)
	END_COM_MAP()
public:
	StackLogger& operator=(const StackLogger&) = delete;

	HRESULT FinalConstruct();
public:
	HRESULT STDMETHODCALLTYPE get_Enabled(BOOL *enabled) override;
	HRESULT STDMETHODCALLTYPE put_Enabled(BOOL enabled) override;
	HRESULT STDMETHODCALLTYPE get_OperationLoggingEnabled(BOOL *enabled) override;
	HRESULT STDMETHODCALLTYPE put_OperationLoggingEnabled(BOOL enabled) override;
	HRESULT STDMETHODCALLTYPE get_MaximumStackDepth(LONG *depth) override;
	HRESULT STDMETHODCALLTYPE put_MaximumStackDepth(LONG depth) override;
	HRESULT STDMETHODCALLTYPE get_HasData(BOOL *result) override;
	HRESULT STDMETHODCALLTYPE get_HasOperationLogs(BOOL *result) override;
	HRESULT STDMETHODCALLTYPE get_Data(IStackData **data) override;
	HRESULT STDMETHODCALLTYPE get_OperationLogs(SAFEARRAY **logs) override;
	HRESULT STDMETHODCALLTYPE ClearData() override;
	HRESULT STDMETHODCALLTYPE ClearOperationLogs() override;
protected:
	StackLogger();
	~StackLogger() override;
private: // Helpers.
	LONG VectoredExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo);
	VOID WriteLog(LPCWSTR pszFormat, ...) noexcept override;
private: // Private Types.
	struct thread_data
	{
		CComPtr<IStackData> stack_data;
		CInterfaceArray<IOperationLog> logs;
		bool in_veh;

		thread_data() : in_veh(false) {}
	};
private: // Member objects. The order of it is significant. It must be construct and destruct in the correct order.
	thread_storage<thread_data> tls;
	std::atomic<bool> enabled;
	std::atomic<bool> logging;
	std::shared_mutex dconf_mtx;
	exception_dumper::config dconf;
	module_handle dmod;
	callback_thunk vthunk;
	PVOID veh;
};
