using System;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    [Guid("669855E8-B4ED-46CB-9118-A80FA42A7F57"), InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    public interface IGlobalConfiguration
    {
        bool EnabledExceptionDumper
        {
            [return: MarshalAs(UnmanagedType.VariantBool)]
            get;
            [param: MarshalAs(UnmanagedType.VariantBool)]
            set;
        }

        bool LogExceptionDumping
        {
            [return: MarshalAs(UnmanagedType.VariantBool)]
            get;
            [param: MarshalAs(UnmanagedType.VariantBool)]
            set;
        }

        int MaximumStackDepth
        {
            [return: MarshalAs(UnmanagedType.I4)]
            get;
            [param: MarshalAs(UnmanagedType.I4)]
            set;
        }

        void Apply();
    }
}
