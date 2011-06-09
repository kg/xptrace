using System;
using NUnit.Framework;

namespace XPTrace {
    [TestFixture]
    public class Tests {
        [Test]
        public void SimpleTest () {
            var output = Util.RunProcess("simpletest.exe");
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
    }
}
