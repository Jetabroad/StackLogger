// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    sealed unsafe class ActivationCookie : IDisposable
    {
        readonly UIntPtr cookie;
        bool disposed;

        public ActivationCookie(UIntPtr cookie)
        {
            this.cookie = cookie;
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

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DeactivateActCtx(uint dwFlags, UIntPtr ulCookie);
    }
}
