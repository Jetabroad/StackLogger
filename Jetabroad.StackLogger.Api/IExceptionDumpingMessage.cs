using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("BCFF3103-E5B8-4807-9F36-E274E619CE0E"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IExceptionDumpingMessage
    {
        double Time
        {
            [return: MarshalAs(UnmanagedType.R8)]
            get;
        }

        string Message
        {
            [return: MarshalAs(UnmanagedType.BStr)]
            get;
        }
    }
}
