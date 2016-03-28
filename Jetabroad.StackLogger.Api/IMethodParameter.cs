using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("CF4C6D5F-3758-4812-8A3D-A2EB0956BB5D"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IMethodParameter
    {
        string Name
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }

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
