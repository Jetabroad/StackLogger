using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    public sealed class StackLoggerInstaller
    {
        const string LoggerFileName = "Jetabroad.StackLogger.Logger.dll";

        /// <summary>
        /// Install StackLogger on the current process. For ASP.NET application, use <see cref="InstallForWeb(string, Assembly)"/> instead.
        /// </summary>
        /// <returns>
        /// <see cref="IStackLogger"/> instance for controlling and retrieve exception information.
        /// </returns>
        public IStackLogger Install()
        {
            using (var activationContext = new Win32ActivationContext(GetLoggerPath(), resourceId: 2))
            using (activationContext.Activate())
            {
                return (IStackLogger)new StackLogger();
            }
        }

        /// <summary>
        /// Install StackLogger on current ASP.NET process. For other application type, use <see cref="Install"/> instead.
        /// Shadow copy DLL 
        /// </summary>
        /// <param name="applicationRoot">
        /// Path of current web application.
        /// </param>
        /// <returns>
        /// <see cref="IStackLogger"/> instance for controlling and retrieve exception information.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="applicationRoot"/> is <c>null</c>.
        /// </exception>
        public IStackLogger InstallForWeb(string applicationRoot)
        {
            if (applicationRoot == null)
            {
                throw new ArgumentNullException(nameof(applicationRoot));
            }

            // Unload old logger first.
            CoFreeUnusedLibrariesEx(0, 0);
            CoFreeUnusedLibraries();

            // Copy logger to the same path as API assembly.
            var destinationDirectory = Path.GetDirectoryName(typeof(StackLoggerInstaller).Assembly.Location);

            if (!destinationDirectory.StartsWith(applicationRoot, StringComparison.OrdinalIgnoreCase))
            {
                var source = Path.Combine(applicationRoot, "bin", LoggerFileName);
                var destination = Path.Combine(destinationDirectory, LoggerFileName);
                File.Copy(source, destination, true);
            }

            return Install();
        }

        static string GetLoggerPath()
        {
            var assemblyDirectory = Path.GetDirectoryName(typeof(StackLoggerInstaller).Assembly.Location);
            return Path.Combine(assemblyDirectory, LoggerFileName);
        }

        [DllImport("ole32.dll")]
        static extern void CoFreeUnusedLibraries();

        [DllImport("combase.dll")]
        static extern void CoFreeUnusedLibrariesEx(uint dwUnloadDelay, uint dwReserved);
    }
}
