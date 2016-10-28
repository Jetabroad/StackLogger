using System;
using System.ComponentModel;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    sealed unsafe class Win32ActivationContext : IDisposable
    {
        //list of all available flags
        //more information https://msdn.microsoft.com/en-us/library/windows/desktop/aa374149(v=vs.85).aspx
        const uint ACTCTX_FLAG_PROCESSOR_ARCHITECTURE_VALID = 0x00000001;
        const uint ACTCTX_FLAG_LANGID_VALID = 0x00000002;
        const uint ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID = 0x00000004;
        const uint ACTCTX_FLAG_RESOURCE_NAME_VALID = 0x00000008;
        const uint ACTCTX_FLAG_SET_PROCESS_DEFAULT = 0x00000010;
        const uint ACTCTX_FLAG_APPLICATION_NAME_VALID = 0x00000020;
        const uint ACTCTX_FLAG_SOURCE_IS_ASSEMBLYREF = 0x00000040;
        const uint ACTCTX_FLAG_HMODULE_VALID = 0x00000080;

        readonly IntPtr handle;
        bool disposed;

        public Win32ActivationContext(string binaryFile, ushort resourceId)
        {
            fixed (char* ptr = binaryFile)
            {
                var context = new ACTCTX()
                {
                    cbSize = (uint)sizeof(ACTCTX),
                    dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID,
                    lpSource = ptr,
                    lpResourceName = (char*)resourceId
                };
                if ((this.handle = CreateActCtx(&context)) == new IntPtr(-1))
                {
                    throw new Win32Exception();
                }
            }
        }

        ~Win32ActivationContext()
        {
            Dispose(false);
        }

        public ActivationCookie Activate()
        {
            UIntPtr cookie;
            if (!ActivateActCtx(this.handle, &cookie))
            {
                throw new Win32Exception();
            }

            return new ActivationCookie(cookie);
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

            ReleaseActCtx(this.handle);

            this.disposed = true;
        }
       //Import Win32 DLL and use function 
        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool ActivateActCtx(IntPtr hActCtx, UIntPtr* lpCookie);

        [DllImport("kernel32.dll", SetLastError = true, EntryPoint = "CreateActCtxW")]
        static extern IntPtr CreateActCtx(ACTCTX* pActCtx);

        [DllImport("kernel32.dll")]
        static extern void ReleaseActCtx(IntPtr hActCtx);

        [StructLayout(LayoutKind.Sequential)]
        struct ACTCTX
        {
            public uint cbSize;
            public uint dwFlags;
            public char* lpSource;
            public ushort wProcessorArchitecture;
            public ushort wLangId;
            public char* lpAssemblyDirectory;
            public char* lpResourceName;
            public char* lpApplicationName;
            public void* hModule;
        }
    }
}
