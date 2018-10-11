// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    /// <summary>
    /// COM class that implemented <see cref="IStackLogger"/>.
    /// </summary>
    /// <example>
    /// <code>
    /// var stackLogger = (IStackLogger)new StackLogger();
    /// </code>
    /// </example>
    [ComImport]
    [Guid("525AEEF8-4464-4E9A-BFCC-CC61277E1D5E")]
    public class StackLogger
    {
    }
}
