// Copyright (c) 2018 Jetabroad (Thailand) Co., Ltd. All Rights Reserved.
// Licensed under the MIT license. See the LICENSE.md file in the project root for license information.
using System;
using System.Globalization;
using System.Runtime.InteropServices;
using NUnit.Framework;

namespace Jetabroad.StackLogger.Api.Testing
{
    [TestFixture]
    public sealed class StackLoggerTest
    {
        IStackLogger stackLogger;

        [SetUp]
        public void SetUp()
        {
            var installer = new StackLoggerInstaller();
            stackLogger = installer.Install();
        }

        [TearDown]
        public void TearDown()
        {
            Marshal.ReleaseComObject(stackLogger); // Force destroy COM object right now.
        }

        [Test]
        public void Enabled_DefaultValue_IsTrue()
        {
            Assert.That(stackLogger.Enabled, Is.True);
        }

        [TestCase(true)]
        [TestCase(false)]
        public void Enabled_ChangeValue_ValueMustBeChanged(bool value)
        {
            stackLogger.Enabled = value;
            Assert.That(stackLogger.Enabled, Is.EqualTo(value));
        }

        [Test]
        public void OperationLoggingEnabled_DefaultValue_IsFalse()
        {
            Assert.That(stackLogger.InternalLoggingEnabled, Is.False);
        }

        [TestCase(true)]
        [TestCase(false)]
        public void OperationLoggingEnabled_ChangeValue_ValueMustBeChanged(bool value)
        {
            stackLogger.InternalLoggingEnabled = value;
            Assert.That(stackLogger.InternalLoggingEnabled, Is.EqualTo(value));
        }

        [Test]
        public void MaximumStackDepth_DefaultValue_Is3()
        {
            Assert.That(stackLogger.MaximumStackDepth, Is.EqualTo(3));
        }

        [TestCase(10)]
        [TestCase(3)]
        [TestCase(5)]
        public void MaximumStackDepth_ChangeValue_ValueMustBeChanged(int value)
        {
            stackLogger.MaximumStackDepth = value;
            Assert.That(stackLogger.MaximumStackDepth, Is.EqualTo(value));
        }

        [Test]
        [ExpectedException(ExpectedException = typeof(ArgumentException))]
        public void MaximumStackDepth_ChangeToNegative_MustThrowException()
        {
            stackLogger.MaximumStackDepth = -1;
        }

        [Test]
        public void Enabled_ValueIsDisabled_HasDataIsFalse()
        {
            // Set Configurations.
            stackLogger.Enabled = false;

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.False);
            }
        }

        [Test]
        public void OperationLoggingEnabled_ValueIsFalse_HasOperationLogsIsFalse()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.InternalLoggingEnabled = false;

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(stackLogger.HasOperationLogs, Is.False);
            }
        }

        [TestCase(5)]
        [TestCase(0)]
        [TestCase(10)]
        public void MaximumStackDepth_VariousValue_DumpedFramesAtMostValue(int depth)
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumStackDepth = depth;

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.True);
                Assert.That(stackLogger.Data.Frames, Has.Length.AtMost(depth));
            }
        }

        [Test]
        public void ClearData_InvokeAfterException_HasDataMustBeFalse()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumStackDepth = 3;

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.True);
            }

            stackLogger.ClearData();
            Assert.That(stackLogger.HasData, Is.False);
        }

        [Test]
        public void Data_ThrowFromInstanceMethod_FirstFrameIsInstanceMethod()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumStackDepth = 1;

            // Run Test.
            try
            {
                throw new Exception();
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.True);
                Assert.That(stackLogger.Data.Frames, Has.Length.EqualTo(1));

                //change name to method name
                Assert.That(stackLogger.Data.Frames[0].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.StackLoggerTest.Data_ThrowFromInstanceMethod_FirstFrameIsInstanceMethod()"));
                Assert.That(stackLogger.Data.Frames[0].Parameters, Has.Length.EqualTo(1));
                Assert.That(stackLogger.Data.Frames[0].Parameters[0].Name, Is.EqualTo("this"));
                Assert.That(stackLogger.Data.Frames[0].Parameters[0].Type, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.StackLoggerTest"));
                Assert.That(stackLogger.Data.Frames[0].Parameters[0].Value as IRawValue, Is.Not.Null);
            }
        }

        [Test]
        public void Data_ThrowFromMethodWithNoArguments_ParametersForThatFrameIsZero()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumStackDepth = 2;

            // Run Test.
            try
            {
                ThrowException<Exception>();
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.True);
                Assert.That(stackLogger.Data.Frames, Has.Length.EqualTo(2));
                Assert.That(stackLogger.Data.Frames[0].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.StackLoggerTest.ThrowException[[System.__Canon, mscorlib]]()"));
                Assert.That(stackLogger.Data.Frames[0].Parameters.Length, Is.EqualTo(0));
                Assert.That(stackLogger.Data.Frames[1].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.StackLoggerTest.Data_ThrowFromMethodWithNoArguments_ParametersForThatFrameIsZero()"));
            }
        }

        [Test]
        public void Data_ThrowFromMethodWithStringArgument_ParameterForThatFrameIsEqualToPassedString()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumStackDepth = 2;

            // Run Test.
            var argument = "Test Message";

            try
            {
                ThrowException<Exception>(argument);
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.True);
                Assert.That(stackLogger.Data.Frames, Has.Length.EqualTo(2));
                Assert.That(stackLogger.Data.Frames[0].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.StackLoggerTest.ThrowException[[System.__Canon, mscorlib]](System.String)"));
                Assert.That(stackLogger.Data.Frames[0].Parameters.Length, Is.EqualTo(1));
                Assert.That(stackLogger.Data.Frames[0].Parameters[0].Name, Is.EqualTo("arg1"));
                Assert.That(stackLogger.Data.Frames[0].Parameters[0].Type, Is.EqualTo("System.String"));

                //ThrowException is static method, therefore we can get it from parameter index 0
                var value = stackLogger.Data.Frames[0].Parameters[0].Value as IStringValue;
                Assert.That(value, Is.Not.Null);
                Assert.That(value.Value, Is.EqualTo(argument));

                Assert.That(stackLogger.Data.Frames[1].Name, Is.EqualTo("Jetabroad.StackLogger.Api.Testing.StackLoggerTest.Data_ThrowFromMethodWithStringArgument_ParameterForThatFrameIsEqualToPassedString()"));
            }
        }

        [Test]
        public void OperationLogs_EnabledOperationLogging_HasAtLeastOneLog()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.InternalLoggingEnabled = true;

            // Run Test.
            var argument = "Test Message";

            try
            {
                ThrowException<ArgumentNullException>(argument);
            }
            catch
            {
                Assert.That(stackLogger.HasData, Is.True);
                Assert.That(stackLogger.HasOperationLogs, Is.True);
                Assert.That(stackLogger.OperationLogs, Has.Length.AtLeast(1));

                foreach (var message in stackLogger.OperationLogs)
                {
                    Console.WriteLine($"[{DateTime.FromOADate(message.Time)}] {message.Message}");
                }
            }
        }

        [TestCase(5)]
        [TestCase(1)]
        [TestCase(10)]
        public void AllData_SetMaximumDataEntriesToSomeValue_NumberOfDataMustBeLessThanOrEqualMaximumDataEntries(int maximumDataEntries)
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumDataEntries = maximumDataEntries;

            // Run Test.
            for (var i = 0; i <= maximumDataEntries + 5; i++)
            {
                try
                {
                    ThrowException<NotImplementedException>(i.ToString(CultureInfo.InvariantCulture));
                }
                catch
                {
                    // We will assert exception details after the loop.
                }
            }

            // Assert.
            Assert.That(stackLogger.HasData, Is.True);
            Assert.That(stackLogger.AllData, Has.Length.LessThanOrEqualTo(maximumDataEntries));

            var expectedArg1 = maximumDataEntries + 5;

            foreach (var data in stackLogger.AllData)
            {
                var arg1 = (IStringValue)data.Frames[0].Parameters[0].Value;

                Assert.That(data.Frames[0].Parameters[0].Name, Is.EqualTo("arg1"));
                Assert.That(arg1.Value, Is.EqualTo(expectedArg1.ToString(CultureInfo.InvariantCulture)));

                expectedArg1--;
            }
        }

        [Test]
        public void AllData_SetMaximumDataEntriesToZero_HasDataMustBeFalse()
        {
            // Set Configurations.
            stackLogger.Enabled = true;
            stackLogger.MaximumDataEntries = 0;

            // Run Test.
            try
            {
                ThrowException<Exception>();
            }
            catch
            {
                // Assert.
                Assert.That(stackLogger.HasData, Is.False);
            }
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void MaximumDataEntries_SetToNegative_MustThrowException()
        {
            stackLogger.MaximumDataEntries = -1;
        }

        [TestCase(0)]
        [TestCase(20)]
        [TestCase(2)]
        public void MaximumDataEntries_ChangeValue_ValueMustBeChanged(int value)
        {
            stackLogger.MaximumDataEntries = value;
            Assert.That(stackLogger.MaximumDataEntries, Is.EqualTo(value));
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
