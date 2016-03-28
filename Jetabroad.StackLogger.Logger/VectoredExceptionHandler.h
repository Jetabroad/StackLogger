// Contains the VEH (Vectored Exception Handler). It is used for listening exceptions in current process.
// For more information, see https://msdn.microsoft.com/en-us/library/windows/desktop/ms681420(v=vs.85).aspx.
#pragma once

#include "Jetabroad.StackLogger.Logger_h.h"

VOID InitialiseVectoredExceptionHandler();
BOOL FinaliseVectoredExceptionHandler();
CComPtr<IDumpedData> GetThreadLastExceptionData();
VOID ClearThreadLastExceptionData();
