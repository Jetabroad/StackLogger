using System;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    public sealed unsafe class ActivationCookie : IDisposable
    {
        readonly UIntPtr cookie;
        bool disposed;

        public ActivationCookie(UIntPtr cookie, Win32ActivationContext owner)
        {
            this.cookie = cookie;
            Owner = owner; // Prevent Win32ActivationContext from GC while ActivationCookie is still alive.
        }

        public Win32ActivationContext Owner
        {
            get;
            private set;
        }

        ~ActivationCookie()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        void Dispose(bool disposing)
        {
            if (this.disposed)
            {
                return;
            }

            if (!DeactivateActCtx(0, this.cookie))
            {
                throw new Win32Exception();
            }

            this.disposed = true;
        }

        [DllImport("kernel32.dll", CallingConvention = CallingConvention.Winapi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DeactivateActCtx(uint dwFlags, UIntPtr ulCookie);
    }
}
