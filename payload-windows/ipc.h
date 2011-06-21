namespace payload {    
    #pragma pack(push)
    #pragma pack(1)

    enum ipcMessageType : unsigned {
        ipc_defineString,
        ipc_setEnabled
    };

    struct ipcMessage {
        ipcMessageType type;

        union {
            struct {
                size_t characterCount;
            } defineString;

            struct {
                unsigned wildcardStringId;
                bool newState;
            } setEnabled;
        };

        unsigned char body[1];
    };

    struct ipcResponse {
        ipcMessageType type;

        union {
            struct {
                unsigned stringId;
            } defineString;

            struct {
                int enabledCount;
            } setEnabled;
        };

        unsigned char body[1];
    };

    #pragma pack(pop)
}