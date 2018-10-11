// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    /// <summary>
    /// Represents a dumped string.
    /// </summary>
    [Guid("DB4DE73F-5DF3-4A28-B900-0C9AC7A47EBB")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IStringValue
    {
        string Value
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
