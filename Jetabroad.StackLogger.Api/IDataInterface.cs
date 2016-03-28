using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("7ACBB0F4-B56D-4D07-AF2F-C786E96A6645"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IDataInterface
    {
        bool HasLastDumped
        {
            [return: MarshalAs(UnmanagedType.VariantBool)]
            get;
        }

        bool HasLastExceptionDumpingLogs
        {
            [return: MarshalAs(UnmanagedType.VariantBool)]
            get;
        }

        void ClearLastDumped();

        [return: MarshalAs(UnmanagedType.Interface)]
        IDumpedData QueryLastDumped();

        [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
        IExceptionDumpingMessage[] QueryLastExceptionDumpingLogs();
    }
}
