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
        /// <summary>
        /// Gets or sets a flags to turn on/off stack dumping when exception occur.
        /// </summary>
        /// <value>
        /// A flags to turn on/off stack dumping when exception occur.
        /// </value>
        bool Enabled
        {
            [return: MarshalAs(UnmanagedType.Bool)]
            get;
            [param: MarshalAs(UnmanagedType.Bool)]
            set;
        }

        /// <summary>
        /// turn on/off internal logging of Stack Logging when we have problem with stack logger itself
        /// </summary>
        bool InternalLoggingEnabled
        {
            [return: MarshalAs(UnmanagedType.Bool)]
            get;
            [param: MarshalAs(UnmanagedType.Bool)]
            set;
        }

        /// <summary>
        /// Gets or sets the maximum number of exception data that will be keep.
        /// </summary>
        /// <value>
        /// The maximum number of exception data that will be keep.
        /// </value>
        /// <exception cref="ArgumentException">
        /// Try to set the property with a negative value.
        /// </exception>
        int MaximumDataEntries
        {
            [return: MarshalAs(UnmanagedType.I4)]
            get;
            [param: MarshalAs(UnmanagedType.I4)]
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

        IStackData[] AllData
        {
            [return: MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_UNKNOWN)]
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
