using System;
using NUnit.Framework;

namespace XPTrace {
    [TestFixture]
    public class Tests {
        [Test]
        public void LoggingTest () {
            var output = Util.RunProcess("loggingtest.exe");
            Assert.AreEqual(
                new[] {
                    "0",
                    "disabled marker hit: 0 'function_with_marker::marker::hit'",
                    "1", "2",
                    "marker hit: 0 'function_with_marker::marker::hit'", 
                    "3", "4"
                },
                output.Split(new[] { Environment.NewLine }, StringSplitOptions.None)
            );
        }

        [Test]
        public void CallbackTest () {
            var output = Util.RunProcess("callbacktest.exe");
            Assert.AreEqual(
                new[] {
                    "0", "1", "2",
                    "marker_callback(0, 1)",
                    "3",
                    "marker_callback(0, 1)",
                    "marker_callback(0, 2)",
                    "4"
                },
                output.Split(new[] { Environment.NewLine }, StringSplitOptions.None)
            );
        }
    }
}
