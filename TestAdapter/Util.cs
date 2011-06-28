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
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Linq;
using NUnit.Framework;

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

    public enum ProcessOutputType {
        Output,
        Error
    }

    // This entire class is an enormous, fiery train wreck because the Process class basically doesn't work. :/
    public class ProcessController : IDisposable {
        // There's a race condition somewhere in the Process class's stream implementation that goes away
        //  if you set this to true sometimes. Fun!
        public const bool Tracing = false;

        protected class OutputWait {
            public readonly ManualResetEventSlim Signal = new ManualResetEventSlim(false);
            public readonly string Text;
            public readonly Regex Regex;

            public OutputWait (Regex regex) {
                Regex = regex;
                Text = null;
            }

            public OutputWait (string text) {
                Text = text;
                Regex = null;
            }

            public bool IsMatch (string text) {
                if (text == null)
                    return false;

                if (Text != null)
                    return text.Contains(Text);
                else
                    return Regex.IsMatch(text);
            }
        }

        public readonly Process Process;
        public readonly List<Tuple<ProcessOutputType, string>> Output = new List<Tuple<ProcessOutputType, string>>();
        public readonly List<Tuple<ProcessOutputType, string>> FilteredOutput = new List<Tuple<ProcessOutputType, string>>();

        protected readonly List<OutputWait> Waits = new List<OutputWait>();

        protected bool _Succeeded = false;
        protected volatile bool _Exited = false;

        public ProcessController (string filename, string arguments) {
            // The process being controlled needs to carefully disable stream buffering and read using getc
            var psi = new ProcessStartInfo(filename, arguments) {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                RedirectStandardInput = true,
                UseShellExecute = false,
                WindowStyle = ProcessWindowStyle.Normal,
                CreateNoWindow = false,
                ErrorDialog = false
            };

            Process = Process.Start(psi);
            Process.StandardInput.AutoFlush = true;
            Process.EnableRaisingEvents = true;
            Process.Exited += OnExit;

            ThreadPool.QueueUserWorkItem(OutputReader);
            ThreadPool.QueueUserWorkItem(ErrorReader);
        }

        protected void OnExit (object sender, EventArgs e) {
            _Exited = true;
        }

        protected void OutputReader (object _) {
            var stream = Process.StandardOutput;

            while (true) {
                var line = stream.ReadLine();

                if (line != null) {
                    if (Tracing)
                        Debug.WriteLine(line);

                    var t = new Tuple<ProcessOutputType, string>(ProcessOutputType.Output, line);

                    lock (Output)
                        Output.Add(t);

                    if (!OnNewText(line))
                        lock (FilteredOutput)
                            FilteredOutput.Add(t);
                } else {
                    break;
                }
            }
        }

        protected void ErrorReader (object _) {
            var stream = Process.StandardError;

            while (true) {
                var line = stream.ReadLine();

                if (line != null) {
                    if (Tracing)
                        Debug.WriteLine(line);

                    var t = new Tuple<ProcessOutputType, string>(ProcessOutputType.Error, line);

                    lock (Output)
                        Output.Add(t);

                    if (!OnNewText(line))
                        lock (FilteredOutput)
                            FilteredOutput.Add(t);
                } else {
                    break;
                }
            }
        }

        protected bool OnNewText (string text) {
            var signals = new List<ManualResetEventSlim>();

            lock (Waits)
                for (int i = Waits.Count - 1; i >= 0; i--) {
                    if (Waits[i].IsMatch(text)) {
                        signals.Add(Waits[i].Signal);
                        Waits.RemoveAt(i);
                    }
                }

            foreach (var s in signals)
                s.Set();

            return signals.Count > 0;
        }

        protected void AddNewWait (OutputWait wait) {
            lock (Waits)
                Waits.Add(wait);

            lock (FilteredOutput) {
                for (int i = 0; i < FilteredOutput.Count; i++) {
                    var ol = FilteredOutput[i];

                    if (wait.IsMatch(ol.Item2)) {
                        FilteredOutput.RemoveAt(i);

                        lock (Waits)
                            Waits.Remove(wait);

                        wait.Signal.Set();

                        return;
                    }
                }
            }
        }

        public ManualResetEventSlim WatchForOutput (string text) {
            var wait = new OutputWait(text);

            AddNewWait(wait);

            return wait.Signal;
        }

        public ManualResetEventSlim WatchForOutput (Regex regex) {
            var wait = new OutputWait(regex);

            AddNewWait(wait);

            return wait.Signal;
        }

        public string AllOutput {
            get {
                lock (Output)
                    return String.Join(Environment.NewLine, (from l in Output select l.Item2));
            }
        }

        public string AllIgnoredOutput {
            get {
                lock (FilteredOutput)
                    return String.Join(Environment.NewLine, (from l in FilteredOutput select l.Item2));
            }
        }

        public void WaitForText (string text, int timeoutMs = 5000) {
            var signal = WatchForOutput(text);

            if (!signal.Wait(timeoutMs))
                throw new TimeoutException(String.Format("The text '{0}' was never written by the process.", text));
        }

        public void Write (string text) {
            Process.StandardInput.Write(text);
        }

        public void Dispose () {
            if (!_Exited)
                try {
                    Process.Kill();
                } catch {
                }

            Process.Dispose();

            if (!_Succeeded) {
                Console.Error.WriteLine("// All output from process:");

                lock (Output)
                foreach (var ol in Output) {
                    if (ol.Item1 == ProcessOutputType.Error)
                        Console.Error.WriteLine(ol.Item2);
                    else
                        Console.WriteLine(ol.Item2);
                }
            }
        }

        public void Succeeded (int timeoutMs = 1000) {
            if (!_Exited)
                Process.WaitForExit(timeoutMs);

            if (!_Exited)
                try {
                    Process.Kill();
                } catch {
                }

            _Succeeded = true;
        }
    }

    public class SimpleFixture {
        public readonly string BinPath;

        public SimpleFixture () {
            var assemblyLocation = Assembly.GetExecutingAssembly().CodeBase;
            var uri = new Uri(assemblyLocation);
            var path = Uri.UnescapeDataString(uri.AbsolutePath);

            if (String.IsNullOrWhiteSpace(path))
                path = Assembly.GetExecutingAssembly().Location;
            else
                path = path.Replace("/", "\\");

            BinPath = Path.GetDirectoryName(path);
        }

        public ProcessController StartProcess (string filename, string arguments = "") {
            return new ProcessController(filename, arguments);
        }

        public void AssertProcessOutput (string filename, string arguments, string[] expectedOutput) {
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
    }
}
