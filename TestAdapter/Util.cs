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
