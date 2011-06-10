/*
The contents of this file are subject to the Mozilla Public License
Version 1.1 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Program Tracing and Instrumentation Toolkit.

The Initial Developer of the Original Code is Mozilla Corporation.

Original Author: Kevin Gadd (kevin.gadd@gmail.com)
*/

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
        public void AddCallbackAfterMarkerInitialized () {
            var output = Util.RunProcess("callbacktest.exe", "0");
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

        [Test]
        public void AddCallbackBeforeMarkerInitialized () {
            var output = Util.RunProcess("callbacktest.exe", "1");
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
