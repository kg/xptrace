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
    public class InteractivePayloadTests : SimpleFixture {
        [Test]
        public void BasicIPC () {
            using (var sp = new ServiceProvider(true))
            using (var p = StartProcess("payloadtest_debug.exe", "0"))
            using (var c = new Controller(p.Process, sp)) {
                p.WaitForText("$0");

                sp.Scheduler.WaitFor(c.InjectPayload());
                sp.Scheduler.WaitFor(c.WaitForPayloadReady());
                sp.Scheduler.WaitFor(c.SetLogging(true));
                sp.Scheduler.WaitFor(c.SetEnabled("*", true));
                p.Write(" ");

                p.WaitForText("$1");
                p.WaitForText("marker hit: 0 'function_with_marker::marker::hit'");
                p.Write(" ");
                p.WaitForText("$2");
                p.WaitForText("marker hit: 0 'function_with_marker::marker::hit'");

                sp.Scheduler.WaitFor(c.SetEnabled("*", false));
                p.Write(" ");

                p.WaitForText("$3");
                p.Write(" ");
                p.WaitForText("$4");

                p.Succeeded();
            }
        }
    }
}
