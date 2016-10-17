using System;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Jetabroad.StackLogger
{
    public sealed class StackLoggerInstaller : IDisposable
    {
        const string LoggerFileName = "Jetabroad.StackLogger.Logger.dll";

        readonly Win32ActivationContext activationContext;

        public StackLoggerInstaller()
        {
            var assemblyDirectory = Path.GetDirectoryName(typeof(StackLoggerInstaller).Assembly.Location);
            var loggerPath = Path.Combine(assemblyDirectory, LoggerFileName);

            activationContext = new Win32ActivationContext(loggerPath, resourceId: 2);
        }

        public void Dispose()
        {
            activationContext.Dispose();
        }

        /// <summary>
        /// Install StackLogger on the current process. For ASP.NET application, use <see cref="InstallForWeb(string, Assembly)"/> instead.
        /// </summary>
        /// <returns>
        /// <see cref="IStackLogger"/> instance for controlling and retrieve exception information.
        /// </returns>
        public IStackLogger Install()
        {
            using (var activation = activationContext.Activate())
            {
                return (IStackLogger)new StackLogger();
            }
        }

        /// <summary>
        /// Install StackLogger on current ASP.NET process. For other application type, use <see cref="Install"/> instead.
        /// </summary>
        /// <param name="applicationRoot">
        /// Path of current web application.
        /// </param>
        /// <param name="applicationAssembly">
        /// Assembly of the current web application.
        /// </param>
        /// <returns>
        /// <see cref="IStackLogger"/> instance for controlling and retrieve exception information.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="applicationRoot"/> or <paramref name="applicationAssembly"/> is <c>null</c>.
        /// </exception>
        public IStackLogger InstallForWeb(string applicationRoot, Assembly applicationAssembly)
        {
            if (applicationRoot == null)
            {
                throw new ArgumentNullException(nameof(applicationRoot));
            }

            if (applicationAssembly == null)
            {
                throw new ArgumentNullException(nameof(applicationAssembly));
            }

            // Unload old logger first.
            CoFreeUnusedLibraries();

            // Copy logger to the same path as web application assembly.
            var shadowBin = Path.GetDirectoryName(applicationAssembly.Location);

            if (!shadowBin.StartsWith(applicationRoot, StringComparison.OrdinalIgnoreCase))
            {
                var source = Path.Combine(applicationRoot, "bin", LoggerFileName);
                var destination = Path.Combine(shadowBin, LoggerFileName);
                File.Copy(source, destination, true);
            }

            return Install();
        }

        [DllImport("ole32.dll")]
        static extern void CoFreeUnusedLibraries();
    }
}
