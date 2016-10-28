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
