#include "PCH.h"
#include "DataInterface.h"
#include "TLS.h"
#include "Jetabroad.StackLogger.Logger_h.h"
#include "Jetabroad.StackLogger.Logger_i.c"
#include "GlobalConfiguration.h"

BEGIN_OBJECT_MAP(g_classes)
	OBJECT_ENTRY(__uuidof(DataInterface), DataInterface)
	OBJECT_ENTRY(__uuidof(GlobalConfiguration), GlobalConfiguration)
END_OBJECT_MAP()

static BOOL Initialise(HINSTANCE hModule)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialise global configurations.
	CAutoPtr<CGlobalConfiguration> pConfig;
	try
	{
		pConfig.Attach(g_pConfig = new CGlobalConfiguration());
	}
	catch (...)
	{
		return FALSE;
	}

	// Initialise TLS.
	CAutoPtr<CTLS> pTls;
	try
	{
		pTls.Attach(g_pTls = new CTLS());
	}
	catch (...)
	{
		return FALSE;
	}

	// Initialise WTL.
	if (FAILED(_Module.Init(g_classes, hModule, &LIBID_StackLogger)))
		return FALSE; // This is a fatal error since the above function is only fail on fatal case.

	// Initialise Data Interface.
	try
	{
		DataInterface::Init();
	}
	catch (...)
	{
		_Module.Term();
		return FALSE;
	}

	pTls.Detach();
	pConfig.Detach();

	return TRUE;
}

static VOID Finalise()
{
	if (!DataInterface::Term())
		return;

	_Module.Term();
	delete g_pTls;
	delete g_pConfig;
}

// COM API. See https://msdn.microsoft.com/en-us/library/windows/desktop/ms690368(v=vs.85).aspx.
STDAPI DllCanUnloadNow()
{
	return _Module.DllCanUnloadNow();
}

// COM API. See https://msdn.microsoft.com/en-us/library/windows/desktop/ms680760(v=vs.85).aspx.
_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
	return _Module.DllGetClassObject(rclsid, riid, ppv);
}

// DLL entry point. See https://msdn.microsoft.com/en-us/library/windows/desktop/ms682583(v=vs.85).aspx.
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID /* lpvReserved */)
{
	static BOOL blInitialised;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (!Initialise(hinstDLL))
			return FALSE;
		blInitialised = TRUE;
		break;
	case DLL_PROCESS_DETACH:
		if (!blInitialised)
			break;
		Finalise();
		break;
	case DLL_THREAD_DETACH:
		g_pTls->DestroyBucket();
		break;
	}

	return TRUE;
}
