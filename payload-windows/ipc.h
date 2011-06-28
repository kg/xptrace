namespace payload {    
    #pragma pack(push)
    #pragma pack(1)

    enum ipcMessageType : unsigned {
        ipc_setLogging,
        ipc_setEnabled
    };

    struct ipcMessage {
        unsigned id;
        ipcMessageType type;

        union {
            struct {
                unsigned char newState;
            } setLogging;

            struct {
                unsigned char newState;
                unsigned wildcardOffset;
                unsigned wildcardLength;
            } setEnabled;
        };
    };

    struct ipcResponse {
        unsigned id;
        ipcMessageType type;

        union {
            struct {
            } setLogging;

            struct {
                int enabledCount;
            } setEnabled;
        };
    };

    #pragma pack(pop)
}