using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using Squared.PE;
using Squared.Task;

namespace XPTrace {
    public class ServiceProvider : IDisposable {
        public readonly TaskScheduler Scheduler;
        public readonly PortableExecutable Payload;

        public ServiceProvider (bool debugPayload) {
            Scheduler = new TaskScheduler(JobQueue.WindowsMessageBased);

            var assembly = Assembly.GetExecutingAssembly();
            using (var stream = assembly.GetManifestResourceStream(
                debugPayload ? 
                    "XPTrace.payload_debug.dll" :
                    "XPTrace.payload.dll"
            ))
                Payload = new PortableExecutable(stream);
        }

        public void Dispose () {
            Scheduler.Dispose();
        }
    }
}
