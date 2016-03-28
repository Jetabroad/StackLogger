using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace Jetabroad.StackLogger
{
    [Guid("E4F88112-CDA2-4762-AC42-349AD538B2A8"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IRawValue
    {
        IStream Value
        {
            [return: MarshalAs(UnmanagedType.Interface)]
            get;
        }
    }
}
