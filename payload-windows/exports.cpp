#include "payload.h"
#include "windows.h"

using namespace xptrace;

HWND g_ownerWindow;
int g_ipcMessageId;

typedef BOOL (WINAPI *pChangeWindowMessageFilter) (
  __in UINT message, __in DWORD dwFlag
);


PAYLOAD_EXPORT(void) payload_main (HWND ownerWindow) {
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
}
