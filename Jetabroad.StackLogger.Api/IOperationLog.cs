// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("BCFF3103-E5B8-4807-9F36-E274E619CE0E")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IOperationLog
    {
        /// <summary>
        /// Gets OLE date of the log.
        /// </summary>
        /// <value>
        /// OLE date of the log.
        /// </value>
        /// <remarks>
        /// Use <see cref="DateTime.FromOADate(double)"/> to convert the value into <see cref="DateTime"/>.
        /// </remarks>
        double Time
        {
            [return: MarshalAs(UnmanagedType.R8)]
            get;
        }

        string Message
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
