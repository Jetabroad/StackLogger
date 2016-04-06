using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    public static class StackLogger
    {
        public const string NativeLibraryFileName = "Jetabroad.StackLogger.Logger.dll";

        static ActivationCookie activatedContext;

        public static IDataInterface DataInterface
        {
            get;
            private set;
        }

        public static string NativeLibraryFilePath
        {
            get { return Path.Combine(Path.GetDirectoryName(typeof(StackLogger).Assembly.Location), NativeLibraryFileName);}
        }

        public static ushort NativeLibraryManifestResourceId
        {
            get { return 2; }
        }

        public static IGlobalConfiguration CreateConfigurations()
        {
            return (IGlobalConfiguration)new GlobalConfiguration();
        }

        /// <summary>
        /// Install StackLogger on current process. For ASP.NET application, use <see cref="InstallForWeb(string, Assembly)"/> instead.
        /// </summary>
        /// <remarks>
        /// This method need to call only once for the process. Usually it should call at the first line of application code.
        /// </remarks>
        public static void Install()
        {
            if (DataInterface != null)
            {
                return;
            }

            if (activatedContext == null)
            {
                var activationContext = new Win32ActivationContext(NativeLibraryFilePath, NativeLibraryManifestResourceId);
                try
                {
                    activatedContext = activationContext.Activate();
                }
                catch
                {
                    activationContext.Dispose();
                    throw;
                }
            }

            try
            {
                DataInterface = (IDataInterface)new DataInterface();
            }
            catch
            {
                activatedContext.Dispose();
                activatedContext.Owner.Dispose();
                activatedContext = null;
                throw;
            }
        }

        /// <summary>
        /// Install StackLogger on current ASP.NET process. For other application, use <see cref="Install"/> instead.
        /// </summary>
        /// <param name="applicationRoot">
        /// Path of current web application.
        /// </param>
        /// <param name="applicationAssembly">
        /// Assembly of current web application.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="applicationRoot"/> or <paramref name="applicationAssembly"/> is <c>null</c>.
        /// </exception>
        /// <remarks>
        /// This method need to call only once. Usually it should call at the first line on Application_Start.
        /// </remarks>
        public static void InstallForWeb(string applicationRoot, Assembly applicationAssembly)
        {
            if (applicationRoot == null)
            {
                throw new ArgumentNullException(nameof(applicationRoot));
            }

            if (applicationAssembly == null)
            {
                throw new ArgumentNullException(nameof(applicationAssembly));
            }

            // Copy native library to the same path as web application assembly.
            CoFreeUnusedLibraries();

            var shadowBin = Path.GetDirectoryName(applicationAssembly.Location);

            if (!shadowBin.StartsWith(applicationRoot, StringComparison.OrdinalIgnoreCase))
            {
                var source = Path.Combine(applicationRoot, "bin", NativeLibraryFileName);
                var destination = Path.Combine(shadowBin, NativeLibraryFileName);
                File.Copy(source, destination, true);
            }

            Install();
        }

        [DllImport("ole32.dll")]
        static extern void CoFreeUnusedLibraries();
    }
}
