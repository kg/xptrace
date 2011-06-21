#include "payload.h"
#include "windows.h"

#include <string>
#include <vector>

using namespace xptrace;

typedef BOOL (WINAPI *pChangeWindowMessageFilter) (
  __in UINT message, __in DWORD dwFlag
);

HWND g_ownerWindow;
int g_ipcMessageId;
std::vector<std::string> g_stringTable;

static unsigned defineString (payload::ipcMessage * message, size_t size) {
    return 0;
}

static void respond (payload::ipcResponse * response, size_t size) {
    PostMessage(
        g_ownerWindow, g_ipcMessageId, 
        reinterpret_cast<WPARAM>(response), 
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

        switch (ipc->type) {
            case payload::ipc_defineString: {
                payload::ipcResponse response;
                response.type = payload::ipc_defineString;
                response.defineString.stringId = defineString(ipc, size);
                respond(&response, sizeof(response));
            } break;

            case payload::ipc_setEnabled: {
                std::string& wildcard = g_stringTable[ipc->setEnabled.wildcardStringId];

                payload::ipcResponse response;
                response.type = payload::ipc_setEnabled;
                response.setEnabled.enabledCount = xptrace_set_markers_enabled(wildcard.c_str(), ipc->setEnabled.newState);
                respond(&response, sizeof(response));
            } break;
        }

        // Free the memory block(s) containing the message
        VirtualFree(ipc, 0, MEM_RELEASE);
    }

    return 0;
}
