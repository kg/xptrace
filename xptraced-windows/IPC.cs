using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace XPTrace {
    public enum IPCMessageType : uint {
        SetLogging,
        SetEnabled
    }

    [StructLayout(LayoutKind.Explicit, Pack = 1, CharSet=CharSet.Ansi)]
    struct IPCMessageHeader {
        [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet=CharSet.Ansi)]
        public struct SetLoggingMessage {
            public byte NewState;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet=CharSet.Ansi)]
        public struct SetEnabledMessage {
            public byte NewState;
            public UInt32 WildcardOffset;
            public UInt32 WildcardLength;
        }

        [FieldOffset(0)]
        public UInt32 ID;
        [FieldOffset(4)]
        public IPCMessageType Type;

        [FieldOffset(8)]
        public SetLoggingMessage SetLogging;
        [FieldOffset(8)]
        public SetEnabledMessage SetEnabled;
    }

    [StructLayout(LayoutKind.Explicit, Pack = 1, CharSet = CharSet.Ansi)]
    public struct IPCResponse {
        [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
        public struct SetLoggingResponse {
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Ansi)]
        public struct SetEnabledResponse {
            public int EnabledCount;
        }

        // ID is stripped out automatically
        [FieldOffset(0)]
        public IPCMessageType Type;

        [FieldOffset(4)]
        public SetLoggingResponse SetLogging;
        [FieldOffset(4)]
        public SetEnabledResponse SetEnabled;
    }

    public static class IPCMessage {
        private static UInt32 TransportStringSize (string text) {
            if (text == null)
                return 0;

            return (UInt32)Encoding.ASCII.GetByteCount(text) + 1;
        }

        private static void WriteTransportString (string text, byte[] buffer, int offset) {
            if (text == null)
                return;

            int chars = Encoding.ASCII.GetBytes(text, 0, text.Length, buffer, offset);
            buffer[offset + chars] = 0;
        }

        public unsafe static byte[] SetLogging (uint id, bool newState) {
            var header = new IPCMessageHeader {
                Type = IPCMessageType.SetLogging,
                ID = id,
                SetLogging = {
                    NewState = newState ? (byte)1 : (byte)0
                }
            };

            var buffer = new byte[Marshal.SizeOf(header)];
            fixed (byte* pBuffer = buffer)
                Marshal.StructureToPtr(header, new IntPtr(pBuffer), false);

            return buffer;
        }

        public unsafe static byte[] SetEnabled (uint id, string wildcard, bool newState) {
            var header = new IPCMessageHeader {
                Type = IPCMessageType.SetEnabled,
                ID = id,
                SetEnabled = {
                    NewState = newState ? (byte)1 : (byte)0,
                    WildcardOffset = 0,
                    WildcardLength = TransportStringSize(wildcard)
                }
            };

            var buffer = new byte[Marshal.SizeOf(header) + header.SetEnabled.WildcardLength];
            fixed (byte* pBuffer = buffer)
                Marshal.StructureToPtr(header, new IntPtr(pBuffer), false);

            WriteTransportString(wildcard, buffer, Marshal.SizeOf(header));

            return buffer;
        }
    }
}
