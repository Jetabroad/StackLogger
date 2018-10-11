// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
#pragma once

#include "ModuleHandle.h"

CComPtr<IXCLRDataProcess> create_dac(HMODULE mod, ICLRDataTarget *prov);
module_handle load_dac();
