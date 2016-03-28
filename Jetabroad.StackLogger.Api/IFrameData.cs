using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("A86EE8ED-B067-4366-8F23-9DD2DB09F385"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IFrameData
    {
        IMethodParameter[] Parameters
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }

        string Name
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
