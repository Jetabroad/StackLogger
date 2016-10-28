using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("3D19AB7B-E022-4418-8358-182C57AFED43")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IObjectDescriptor2 : IObjectDescriptor
    {
        string Name
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
