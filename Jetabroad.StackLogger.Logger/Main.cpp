#include "PCH.h"

#include "GlobalEvents.h"
#include "StackLogger.h"

#include "Jetabroad.StackLogger.Logger_h.h"
#include "Jetabroad.StackLogger.Logger_i.c"

BEGIN_OBJECT_MAP(g_ClassDescriptors)
	OBJECT_ENTRY(__uuidof(StackLogger), StackLogger)
END_OBJECT_MAP()

static VOID Initialise(HINSTANCE hModule)
{
	HRESULT hr;

	// Enable memory leak reporting.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Initialise Base System.
	hr = _Module.Init(g_ClassDescriptors, hModule, &LIBID_StackLogger);
	if (FAILED(hr))
		AtlThrow(hr);
}

static VOID Finalise()
{
	_Module.Term();
}

// COM API. See https://msdn.microsoft.com/en-us/library/windows/desktop/ms690368(v=vs.85).aspx.
STDAPI DllCanUnloadNow()
{
	ATLTRACE2(L"[%u] DllCanUnloadNow() with lock count %d.", GetCurrentThreadId(), _Module.GetLockCount());
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
	static bool initialised;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		try
		{
			Initialise(hinstDLL);
		}
		catch (...)
		{
			return FALSE;
		}

		initialised = true;
		break;
	case DLL_PROCESS_DETACH:
		if (!initialised)
			break;
		Finalise();
		break;
	case DLL_THREAD_DETACH:
		raise_thread_exit();
		break;
	}

	return TRUE;
}
