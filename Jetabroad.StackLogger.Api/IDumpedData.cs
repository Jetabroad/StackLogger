using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("E0B138FC-48D7-4D1E-B6BF-D708EAEC9711"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IDumpedData
    {
        IFrameData[] Frames
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }
    }
}
