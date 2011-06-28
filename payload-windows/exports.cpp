#include "payload.h"
#include "windows.h"
#include "stdio.h"

using namespace xptrace;

typedef BOOL (WINAPI *pChangeWindowMessageFilter) (
  __in UINT message, __in DWORD dwFlag
);

HWND g_ownerWindow;
int g_ipcMessageId;

static void copy (void * dest, const void * src, size_t size) {
    unsigned char * pDest = reinterpret_cast<unsigned char *>(dest);
    const unsigned char * pSrc = reinterpret_cast<const unsigned char *>(src);

    for (unsigned i = 0; i < size; i++) {
        *pDest = *pSrc;
        pDest += 1;
        pSrc += 1;
    }
}

static void respond (payload::ipcResponse * response, size_t size) {
    void * pRegion = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    copy(pRegion, response, size);

    PostMessage(
        g_ownerWindow, g_ipcMessageId, 
        reinterpret_cast<WPARAM>(pRegion), 
        reinterpret_cast<LPARAM>(reinterpret_cast<void *>(size))
    ); 
}

PAYLOAD_EXPORT(int) payload_threadProc (HWND ownerWindow) {
    g_ownerWindow = ownerWindow;
    g_ipcMessageId = RegisterWindowMessage(L"xptrace.IPCMessage");

    // Calling PeekMessage once ensures that our thread has a message queue
    MSG msg;
    PeekMessage(&msg, 0, g_ipcMessageId, g_ipcMessageId, 0);

    // If our integrity level is higher than that of the tracing control process, we
    //  must change the UIPI filter so that IPC messages can make it into our queue
    {
        HMODULE hModule = LoadLibrary(L"user32.dll");

        // UIPI is still relatively new, so only try to call the API if it's available
        pChangeWindowMessageFilter pFn = (pChangeWindowMessageFilter)GetProcAddress(hModule, "ChangeWindowMessageFilter");
        if (pFn)
            pFn(g_ipcMessageId, MSGFLT_ADD);

        FreeLibrary(hModule);
    }

    // Alert the control process that we are ready to receive messages
    PostMessage(ownerWindow, g_ipcMessageId, 0, 0);

    BOOL result;
    while ((result = GetMessage(&msg, 0, g_ipcMessageId, g_ipcMessageId)) != 0) {
        // We've been told to terminate our message queue
        if (result == -1)
            break;

        payload::ipcMessage * ipc = reinterpret_cast<payload::ipcMessage *>(msg.wParam);
        size_t size = reinterpret_cast<size_t>(reinterpret_cast<void *>(msg.lParam));

        payload::ipcResponse response;
        response.id = ipc->id;
        bool responseValid = true;

        switch (ipc->type) {
            case payload::ipc_setLogging: {
                xptrace_set_logging_enabled(ipc->setLogging.newState);

                response.type = payload::ipc_setLogging;
            } break;

            case payload::ipc_setEnabled: {
                auto pWildcard = reinterpret_cast<const char *>(ipc) + 
                    sizeof(payload::ipcMessage) + ipc->setEnabled.wildcardOffset;

                response.type = payload::ipc_setEnabled;
                response.setEnabled.enabledCount = xptrace_set_markers_enabled(
                    pWildcard, ipc->setEnabled.newState
                );
            } break;

            default: {
                MessageBoxA(0, "unknown", "payload", 0);
                responseValid = false;
            } break;
        }

        if (responseValid)
            respond(&response, sizeof(response));

        // Free the memory block(s) containing the message
        VirtualFree(ipc, 0, MEM_RELEASE);
    }

    return 0;
}
