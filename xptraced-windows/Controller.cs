using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using Squared.PE;
using Squared.Task;

namespace XPTrace {
    public class Controller : IDisposable {
        public readonly Process Process;
        public readonly ServiceProvider Services;
        public readonly RPCChannel Channel;

        protected IFuture PayloadReadyFuture;

        public Controller (Process process, ServiceProvider services) {
            Process = process;
            Services = services;
            Channel = new RPCChannel(process, "xptrace.IPCMessage");
        }

        public IEnumerator<object> InjectPayload () {
            var fThreadResult = new Future<int>();
            var fThreadId = new Future<uint>();

            yield return Future.RunInThread(() => {
                PEInjector.Inject(Process, Services.Payload, Channel.Handle, fThreadResult, fThreadId);
            });

            yield return fThreadId;
            Channel.RemoteThreadId = fThreadId.Result;

            PayloadReadyFuture = Channel.Receive();
        }

        public IFuture WaitForPayloadReady () {
            return PayloadReadyFuture;
        }

        protected IFuture SendMessage (byte[] message, uint id) {
            var fResult = Channel.WaitForMessage(id);

            Channel.Send(message);

            return fResult;
        }

        public IFuture SetLogging (bool newState) {
            var id = Channel.GetMessageID();
            var msg = IPCMessage.SetLogging(id, newState);
            return SendMessage(msg, id);
        }

        public IFuture SetEnabled (string wildcard, bool newState) {
            var id = Channel.GetMessageID();
            var msg = IPCMessage.SetEnabled(id, wildcard, newState);
            return SendMessage(msg, id);
        }

        public void Dispose () {
            Channel.Dispose();
        }
    }
}
