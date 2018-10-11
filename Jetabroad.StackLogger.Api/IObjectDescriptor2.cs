// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("3D19AB7B-E022-4418-8358-182C57AFED43")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IObjectDescriptor2
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

        string Name
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
