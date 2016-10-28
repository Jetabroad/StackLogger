using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("0BFD63CA-465E-4A3E-9C1C-78E401888D2C")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IComplexValue
    {
        IObjectDescriptor2[] Fields
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }
    }
}
