using System.IO;

namespace Jetabroad.StackLogger
{
    public static class StackLogger
    {
        static ActivationCookie activatedContext;

        public static IDataInterface DataInterface
        {
            get;
            private set;
        }

        public static string NativeLibraryFileName
        {
            get { return Path.Combine(Path.GetDirectoryName(typeof(StackLogger).Assembly.Location), "Jetabroad.StackLogger.Logger.dll");}
        }

        public static ushort NativeLibraryManifestResourceId
        {
            get { return 2; }
        }

        public static IGlobalConfiguration CreateConfigurations()
        {
            return (IGlobalConfiguration)new GlobalConfiguration();
        }

        public static void Install()
        {
            if (DataInterface != null)
            {
                return;
            }

            if (activatedContext == null)
            {
                var activationContext = new Win32ActivationContext(NativeLibraryFileName, NativeLibraryManifestResourceId);
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
    }
}
