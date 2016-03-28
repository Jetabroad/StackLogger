using System;
using System.Runtime.InteropServices;
using NUnit.Framework;

namespace Jetabroad.StackLogger.Api.Testing
{
    public sealed class GlobalConfigurationTest : StackLoggerTestFixture
    {
        IGlobalConfiguration config;

        [SetUp]
        public void SetUp()
        {
            config = (IGlobalConfiguration)new GlobalConfiguration();
        }

        [TearDown]
        public void TearDown()
        {
            Marshal.ReleaseComObject(config);
        }

        [Test]
        public void TestSetValues()
        {
            config.EnabledExceptionDumper = true;
            Assert.That(config.EnabledExceptionDumper, Is.True);

            config.EnabledExceptionDumper = false;
            Assert.That(config.EnabledExceptionDumper, Is.False);

            config.LogExceptionDumping = true;
            Assert.That(config.LogExceptionDumping, Is.True);

            config.LogExceptionDumping = false;
            Assert.That(config.LogExceptionDumping, Is.False);

            config.MaximumStackDepth = 10;
            Assert.That(config.MaximumStackDepth, Is.EqualTo(10));

            config.MaximumStackDepth = 3;
            Assert.That(config.MaximumStackDepth, Is.EqualTo(3));
        }

        [Test, ExpectedException(ExpectedException = typeof(ArgumentException))]
        public void TestNegativeMaximumStackDepth()
        {
            config.MaximumStackDepth = -1;
        }
    }
}
