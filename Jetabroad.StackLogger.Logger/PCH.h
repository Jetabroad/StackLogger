// This is pre-compiled header. Include any headers that rarely change in this header to reduce compilation time.
// For more information, see https://en.wikipedia.org/wiki/Precompiled_header and https://msdn.microsoft.com/en-us/library/szfdksca.aspx.
#pragma once

#include "PlatformTarget.h"

// Compilation Controlling Macros:

#define WIN32_LEAN_AND_MEAN // Exclude rarely used stub from windows.h to reduce compilation time.

#define _ATL_FREE_THREADED // Our COM objects need to be thread-safe since default threading model for managed world is MTA (Multi-threaded Apartment).
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // Prevent accidentally pass "int" and other type to argument with CString type.
#define _WTL_NO_CSTRING // We will use ATL CString instead.
#define _WTL_NO_WTYPES // We will use ATL implementation instead.

#define _CRTDBG_MAP_ALLOC

// Windows Header Files:

#include <windows.h>

#include <psapi.h>

// CLR Header Files:

#include "xclrdata.h"

// ATL & WTL Header Files:

#include <atlbase.h>
#include <atlstr.h>
#include <atltypes.h>
#include <atlapp.h>

extern CServerAppModule _Module;

#include <atlcomtime.h>
#include <atlsafe.h>
#include <atlcoll.h>
#include <atlmisc.h>
#include <atlpath.h>
#include <atlfile.h>

// C++ Header Files:

#include <atomic>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <system_error>
#include <unordered_map>
#include <utility>
#include <vector>

// C Header Files:

#include <crtdbg.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

// Helper template:
template<class T>
CComPtr<T> create_com() //helper to create COM
{
	CComObject<T> *obj;
	auto hr = CComObject<T>::CreateInstance(&obj);
	if (FAILED(hr))
		AtlThrow(hr);

	return obj;
}

class binary : public std::vector<uint8_t>
{
public:
	binary(std::initializer_list<uint8_t> init) : vector(init) {}
	binary(binary&& other) : vector(std::forward<binary>(other)) {}

	void append(const binary& other)
	{
		append(other.begin(), other.end());
	}

	void append(std::initializer_list<uint8_t> list)
	{
		insert(end(), list);
	}

	void append(const uint8_t *p, size_t n)
	{
		append(p, p + n);
	}

	template<class InputIt>
	void append(InputIt first, InputIt last)
	{
		insert(end(), first, last);
	}
};
