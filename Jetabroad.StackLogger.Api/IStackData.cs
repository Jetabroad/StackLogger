// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("E0B138FC-48D7-4D1E-B6BF-D708EAEC9711")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IStackData
    {
        IFrameData[] Frames
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }
    }
}
