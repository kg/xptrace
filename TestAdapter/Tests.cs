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
        static void AssertProcessOutput (string filename, string arguments, string[] expectedOutput) {
            long started = DateTime.UtcNow.Ticks;
            var output = Util.RunProcess(filename, arguments);
            long elapsed = DateTime.UtcNow.Ticks - started;

            try {
                Assert.AreEqual(
                    expectedOutput,
                    output.Split(new[] { Environment.NewLine }, StringSplitOptions.None)
                );
                Console.WriteLine("---- '{0}' '{1}' ran in {2:00.000}s. ----", filename, arguments, TimeSpan.FromTicks(elapsed).TotalSeconds);
            } catch {
                Console.WriteLine("---- '{0}' '{1}' output ----", filename, arguments);
                Console.WriteLine(output);
                Console.WriteLine("----");
                throw;
            }
        }

        [Test]
        public void LoggingTest () {
            AssertProcessOutput(
                "loggingtest_debug.exe", "0",
                new[] {
                    "0", "disabled marker hit: 0 'function_with_marker::marker::hit'",
                    "1", 
                    "2", "marker hit: 0 'function_with_marker::marker::hit'",
                    "3", "4", "0", "1", "2", 
                    "marker hit: 0 'function_with_marker::marker::hit'",
                    "3", "4"
                }
            );

            AssertProcessOutput(
                "loggingtest_release.exe", "0",
                new[] {
                    "0", "disabled marker hit: 0 'function_with_marker::marker::hit'",
                    "1", "disabled marker hit: 0 'function_with_marker::marker::hit'",
                    "2", "marker hit: 0 'function_with_marker::marker::hit'",
                    "3", "disabled marker hit: 0 'function_with_marker::marker::hit'",
                    "4", "0", "1", "2", "marker hit: 0 'function_with_marker::marker::hit'",
                    "3", "4"
                }
            );
        }

        [Test]
        public void FunctionTiming () {
            AssertProcessOutput(
                "loggingtest_debug.exe", "1",
                new[] {
                    "0", "marker hit: 0 'timed_function::enter'", 
                    "timed_function(1)", 
                    "marker hit: 1 'timed_function::exit'", 
                    "1", "marker hit: 0 'timed_function::enter'", 
                    "timed_function(2)", 
                    "marker hit: 0 'timed_function::enter'", 
                    "timed_function(1)", 
                    "marker hit: 1 'timed_function::exit'", 
                    "marker hit: 1 'timed_function::exit'", "2"
                }
            );

            AssertProcessOutput(
                "loggingtest_release.exe", "1",
                new[] {
                    "0", "marker hit: 0 'timed_function::enter'", 
                    "timed_function(1)", 
                    "marker hit: 1 'timed_function::exit'", 
                    "1", "marker hit: 0 'timed_function::enter'", 
                    "timed_function(2)", 
                    "marker hit: 0 'timed_function::enter'", 
                    "timed_function(1)", 
                    "marker hit: 1 'timed_function::exit'", 
                    "marker hit: 1 'timed_function::exit'", "2"
                }
            );
        }

        [Test]
        public void ManyTimingCalls () {
            int num_iterations = 1024 * 32;
            AssertProcessOutput(
                "loggingtest_debug.exe", String.Format("2 {0}", num_iterations),
                new[] {
                    String.Format("Running {0} iteration(s) with markers enabled...", num_iterations),
                    String.Format("Running {0} iteration(s) with markers disabled...", num_iterations),
                    "ok."
                }
            );

            num_iterations = 1024 * 1024 * 4;
            AssertProcessOutput(
                "loggingtest_release.exe", String.Format("2 {0}", num_iterations),
                new[] {
                    String.Format("Running {0} iteration(s) with markers enabled...", num_iterations),
                    String.Format("Running {0} iteration(s) with markers disabled...", num_iterations),
                    "ok."
                }
            );
        }

        [Test]
        public void AddCallbackAfterMarkerInitialized () {
            AssertProcessOutput(
                "callbacktest_debug.exe", "0",
                new[] {
                    "0", "1", "2",
                    "marker_callback(0, 1)",
                    "3",
                    "marker_callback(0, 1)",
                    "marker_callback(0, 2)",
                    "4"
                }
            );

            AssertProcessOutput(
                "callbacktest_release.exe", "0",
                new[] {
                    "0", "1", "2",
                    "marker_callback(0, 1)",
                    "3",
                    "marker_callback(0, 1)",
                    "marker_callback(0, 2)",
                    "4"
                }
            );
        }

        [Test]
        public void AddCallbackBeforeMarkerInitialized () {
            AssertProcessOutput(
                "callbacktest_debug.exe", "1",
                new[] {
                    "0", "marker_callback(0, 3)", 
                    "1", "marker_callback(0, 3)", 
                    "2", "marker_callback(0, 3)", 
                    "marker_callback(0, 4)", 
                    "3", "marker_callback(0, 3)", 
                    "marker_callback(0, 4)", "4"
                }
            );

            AssertProcessOutput(
                "callbacktest_release.exe", "1",
                new[] {
                    "0", "marker_callback(0, 3)", 
                    "1", "marker_callback(0, 3)", 
                    "2", "marker_callback(0, 3)", 
                    "marker_callback(0, 4)", 
                    "3", "marker_callback(0, 3)", 
                    "marker_callback(0, 4)", "4"
                }
            );
        }

        [Test]
        public void RemoveCallback () {
            AssertProcessOutput(
                "callbacktest_debug.exe", "2",
                new[] {
                    "0",
                    "marker_callback(0, 3)",
                    "1", "2"
                }
            );

            AssertProcessOutput(
                "callbacktest_release.exe", "2",
                new[] {
                    "0",
                    "marker_callback(0, 3)",
                    "1", "2"
                }
            );
        }
    }
}
