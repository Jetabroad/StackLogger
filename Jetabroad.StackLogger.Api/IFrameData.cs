// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("A86EE8ED-B067-4366-8F23-9DD2DB09F385")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IFrameData
    {
        IObjectDescriptor2[] Parameters
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }

        string Name
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
