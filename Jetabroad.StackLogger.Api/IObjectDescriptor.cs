// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("E7CEBDB1-E8A5-40F9-BBEB-DC0B376D3444")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IObjectDescriptor
    {
        string Type
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }

        object Value
        {
            [return: MarshalAs(UnmanagedType.IUnknown)]
            get;
        }
    }
}
