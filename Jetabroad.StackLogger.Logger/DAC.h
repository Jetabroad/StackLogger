#pragma once

#include "ModuleHandle.h"

CComPtr<IXCLRDataProcess> create_dac(HMODULE mod, ICLRDataTarget *prov);
module_handle load_dac();
