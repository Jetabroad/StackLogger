using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("E4F88112-CDA2-4762-AC42-349AD538B2A8")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IRawValue
    {
        byte[] Value
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UI1)]
            get;
        }
    }
}
