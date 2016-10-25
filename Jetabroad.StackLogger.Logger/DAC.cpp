#include "PCH.h"
#include "DAC.h"

#ifdef _WIN64
#define CLR_DIRECTORY L"Microsoft.NET\\Framework64\\v4.0.30319"
#else
#define CLR_DIRECTORY L"Microsoft.NET\\Framework\\v4.0.30319"
#endif

#define DAC_LIBRARY L"mscordacwks.dll"

// Data Access Component to access internal CLR and use clrdata.idl and xclrdata.idl from PCH.h

CComPtr<IXCLRDataProcess> create_dac(HMODULE mod, ICLRDataTarget *prov)
{
	auto func = reinterpret_cast<PFN_CLRDataCreateInstance>(GetProcAddress(mod, "CLRDataCreateInstance"));
	if (!func)
		throw std::system_error(GetLastError(), std::system_category());

	CComPtr<IXCLRDataProcess> dac;
	auto hr = func(__uuidof(IXCLRDataProcess), prov, reinterpret_cast<void **>(&dac));
	if (FAILED(hr))
		AtlThrow(hr);

	return dac;
}

module_handle load_dac()
{
	// Get location of C:\Windows.
	auto nbuf = GetSystemWindowsDirectory(NULL, 0);
	if (!nbuf)
		throw std::system_error(GetLastError(), std::system_category());

	auto buf = std::make_unique<WCHAR[]>(nbuf);
	if (!GetSystemWindowsDirectory(buf.get(), nbuf))
		throw std::system_error(GetLastError(), std::system_category());

	std::tr2::sys::path path(buf.get());

	// Construct full path of "C:\Windows\Microsoft.NET\Framework[64]\v4.0.30319\mscordacwks.dll".
	path.append(CLR_DIRECTORY);
	path.append(DAC_LIBRARY);

	// Load mscordacwks.dll.
	auto mod = LoadLibrary(path.c_str());
	if (!mod)
		throw std::system_error(GetLastError(), std::system_category());

	return module_handle(mod);
}
