using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    /// <summary>
    /// Interface to controlling StackLogger.
    /// </summary>
    [Guid("B0B7FDE7-A41E-4264-A9B9-C073C14E4680")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IStackLogger
    {
        bool Enabled
        {
            [return: MarshalAs(UnmanagedType.Bool)]
            get;
            [param: MarshalAs(UnmanagedType.Bool)]
            set;
        }

        bool InternalLoggingEnabled
        {
            [return: MarshalAs(UnmanagedType.Bool)]
            get;
            [param: MarshalAs(UnmanagedType.Bool)]
            set;
        }

        int MaximumStackDepth
        {
            [return: MarshalAs(UnmanagedType.I4)]
            get;
            [param: MarshalAs(UnmanagedType.I4)]
            set;
        }

        bool HasData
        {
            [return: MarshalAs(UnmanagedType.Bool)]
            get;
        }

        bool HasOperationLogs
        {
            [return: MarshalAs(UnmanagedType.Bool)]
            get;
        }

        IStackData Data
        {
            [return: MarshalAs(UnmanagedType.Interface)]
            get;
        }

        IOperationLog[] OperationLogs
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
            get;
        }

        void ClearData();
        void ClearOperationLogs();
    }
}
