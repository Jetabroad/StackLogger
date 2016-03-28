using System;
using System.Runtime.InteropServices;
using NUnit.Framework;

namespace Jetabroad.StackLogger.Api.Testing
{
    public sealed class DataInterfaceTest : StackLoggerTestFixture
    {
        IDataInterface dataInterface;

        [SetUp]
        public void SetUp()
        {
            dataInterface = (IDataInterface)new DataInterface();
        }

        [TearDown]
        public void TearDown()
        {
            dataInterface.ClearLastDumped();
            Marshal.ReleaseComObject(dataInterface);
        }

        [Test]
        public void TestDisableDumper()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = false;
            config.Apply();

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(dataInterface.HasLastDumped, Is.False);
                Assert.That(dataInterface.HasLastExceptionDumpingLogs, Is.False);
            }
        }

        [Test]
        public void TestDisableLogging()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = true;
            config.LogExceptionDumping = false;
            config.Apply();

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(dataInterface.HasLastDumped, Is.True);
                Assert.That(dataInterface.HasLastExceptionDumpingLogs, Is.False);
            }
        }

        [Test]
        public void TestMaximumFrameDepth()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = true;
            config.LogExceptionDumping = true;
            config.MaximumStackDepth = 0;
            config.Apply();

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                PrintExceptionDumpingLogs(dataInterface);
                Assert.That(dataInterface.HasLastDumped, Is.True);
                Assert.That(dataInterface.QueryLastDumped().Frames, Has.Length.EqualTo(0));
            }
        }

        [Test]
        public void TestClearLastDumping()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = true;
            config.LogExceptionDumping = false;
            config.MaximumStackDepth = 3;
            config.Apply();

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(dataInterface.HasLastDumped, Is.True);
            }

            dataInterface.ClearLastDumped();
            Assert.That(dataInterface.HasLastDumped, Is.False);
        }

        [Test]
        public void TestObjectInstance()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = true;
            config.LogExceptionDumping = true;
            config.MaximumStackDepth = int.MaxValue;
            config.Apply();

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                PrintExceptionDumpingLogs(dataInterface);
                Assert.IsTrue(dataInterface.HasLastDumped);

                var exceptionData = dataInterface.QueryLastDumped();
                var frames = exceptionData.Frames;

                Assert.That(frames, Has.Length.AtLeast(1));
                Assert.That(frames[0].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.DataInterfaceTest.TestObjectInstance()"));

                Assert.AreEqual(1, frames[0].Parameters.Length);

                Assert.That(frames[0].Parameters[0].Name, Is.EqualTo("this"));
                Assert.That(frames[0].Parameters[0].Type, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.DataInterfaceTest"));
                Assert.That(frames[0].Parameters[0].Value as IRawValue, Is.Not.Null);
            }
        }

        [Test]
        public void TestNoArgument()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = true;
            config.LogExceptionDumping = true;
            config.MaximumStackDepth = int.MaxValue;
            config.Apply();

            // Run Test.
            try
            {
                ThrowException<Exception>();
            }
            catch
            {
                PrintExceptionDumpingLogs(dataInterface);
                Assert.IsTrue(dataInterface.HasLastDumped);

                var exceptionData = dataInterface.QueryLastDumped();
                var frames = exceptionData.Frames;

                Assert.That(frames, Has.Length.AtLeast(2));
                Assert.That(frames[0].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.DataInterfaceTest.ThrowException[[System.__Canon, mscorlib]]()"));
                Assert.That(frames[1].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.DataInterfaceTest.TestNoArgument()"));

                Assert.That(frames[0].Parameters.Length, Is.EqualTo(0));
            }
        }

        [Test]
        public void TestStringArgument()
        {
            // Set Configurations.
            var config = (IGlobalConfiguration)new GlobalConfiguration();
            config.EnabledExceptionDumper = true;
            config.LogExceptionDumping = true;
            config.MaximumStackDepth = int.MaxValue;
            config.Apply();

            // Run Test.
            var argument = "Test Message";

            try
            {
                ThrowException<Exception>(argument);
            }
            catch
            {
                PrintExceptionDumpingLogs(dataInterface);
                Assert.IsTrue(dataInterface.HasLastDumped);

                var exceptionData = dataInterface.QueryLastDumped();
                var frames = exceptionData.Frames;

                Assert.That(frames, Has.Length.AtLeast(2));
                Assert.That(frames[0].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.DataInterfaceTest.ThrowException[[System.__Canon, mscorlib]](System.String)"));
                Assert.That(frames[1].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.DataInterfaceTest.TestStringArgument()"));

                Assert.That(frames[0].Parameters.Length, Is.EqualTo(1));
                Assert.That(frames[0].Parameters[0].Name, Is.EqualTo("arg1"));
                Assert.That(frames[0].Parameters[0].Type, Is.EqualTo("System.String"));

                var value = frames[0].Parameters[0].Value as IStringValue;

                Assert.That(value, Is.Not.Null);
                Assert.That(value.Value, Is.EqualTo(argument));
            }
        }

        static void PrintExceptionDumpingLogs(IDataInterface dataInterface)
        {
            if (!dataInterface.HasLastExceptionDumpingLogs)
            {
                return;
            }

            foreach (var message in dataInterface.QueryLastExceptionDumpingLogs())
            {
                Console.WriteLine("[{0}] {1}", DateTime.FromOADate(message.Time), message.Message);
            }
        }

        static void ThrowException<TException>() where TException : Exception, new()
        {
            throw new TException();
        }

        static void ThrowException<TException>(string arg1) where TException : Exception, new()
        {
            throw new TException();
        }
    }
}
