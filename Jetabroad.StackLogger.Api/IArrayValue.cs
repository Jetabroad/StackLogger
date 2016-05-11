using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("ABAFE622-7FE8-40F4-9F33-17406169A604")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IArrayValue
    {
        object[] Values
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }
    }
}
