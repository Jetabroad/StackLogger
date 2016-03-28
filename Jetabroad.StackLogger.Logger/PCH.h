// This is pre-compiled header. Include any headers that rarely change in this header to reduce compilation time.
// For more information, see https://en.wikipedia.org/wiki/Precompiled_header and https://msdn.microsoft.com/en-us/library/szfdksca.aspx.
#pragma once

#include "PlatformTarget.h"

// Windows Header Files:

#define WIN32_LEAN_AND_MEAN // Exclude rarely used stub from windows.h to reduce compilation time.
#include <windows.h>

// CLR Header Files:

#include "xclrdata.h"

// ATL & WTL Header Files:

#define _ATL_FREE_THREADED // Our COM objects need to be thread-safe since default threading model for managed world is MTA (Multi-threaded Apartment).
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // Prevent accidentally pass "int" and other type to argument with CString type.
#define _WTL_NO_CSTRING // We will use ATL CString instead.
#define _WTL_NO_WTYPES // We will use ATL implementation instead.

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

// C Header Files:

#include <stdarg.h>
#include <time.h>
