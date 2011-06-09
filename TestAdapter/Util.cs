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
using System.Diagnostics;
using System.Text;

namespace XPTrace {
    public static class Util {
        public static string RunProcess (string filename, string arguments = "") {
            var psi = new ProcessStartInfo(filename, arguments) {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                WindowStyle = ProcessWindowStyle.Hidden,
                CreateNoWindow = true,
                ErrorDialog = false
            };

            var output = new StringBuilder();

            using (var process = Process.Start(psi)) {
                process.ErrorDataReceived += (sender, e) => {
                    lock (output)
                        output.AppendLine(e.Data);
                };
                process.OutputDataReceived += (sender, e) => {
                    lock (output)
                        output.AppendLine(e.Data);
                };

                process.BeginErrorReadLine();
                process.BeginOutputReadLine();

                process.WaitForExit();
            }

            return output.ToString().Trim();
        }
    }
}
