// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("ABAFE622-7FE8-40F4-9F33-17406169A604")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IArrayValue
    {
        object[] Values
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }
    }
}
