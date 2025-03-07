// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
// This file used for control target platform. It will enable or disable Win32 APIs according to _WIN32_WINNT.
#pragma once

#include <winsdkver.h>
#define _WIN32_WINNT 0x0601 // Windows 7 or Server 2008 R2. We cannot set to Server 2012 since our build server is Server 2008 R2.
#include <sdkddkver.h>
