using System.Runtime.InteropServices;
using NUnit.Framework;

namespace Jetabroad.StackLogger.Api.Testing
{
    [TestFixture]
    public abstract class StackLoggerTestFixture
    {
        static Win32ActivationContext context;
        static ActivationCookie contextActivationCookie;

        [TestFixtureSetUp]
        public static void FixtureSetUp()
        {
            context = new Win32ActivationContext(StackLogger.NativeLibraryFilePath, StackLogger.NativeLibraryManifestResourceId);
            contextActivationCookie = context.Activate();
        }

        [TestFixtureTearDown]
        public static void FixtureTearDown()
        {
            // Disable StackDumper for othet tests.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = false;
            config.Apply();
            Marshal.ReleaseComObject(config);

            // Free unmanaged resources.
            contextActivationCookie.Dispose();
            context.Dispose();
        }
    }
}
