using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("DB4DE73F-5DF3-4A28-B900-0C9AC7A47EBB"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IStringValue
    {
        string Value
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
