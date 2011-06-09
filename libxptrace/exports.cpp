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

#include "xptrace.h"
#include "windows.h"

using namespace xptrace;

XPTRACE_EXPORT(void) xptrace_initialize () {
}

XPTRACE_EXPORT(markerid) xptrace_register_marker (const char * name, const void * return_address) {
    markerid newId = markers.size();
    marker newMarker = {
        string(name), return_address, true
    };
    markers.push_back(newMarker);

    xptrace_set_marker_enabled_by_id(newId, false);

    return newId;
}

struct ChangedPageProtection {
    HANDLE process;
    LPVOID address;
    DWORD size;
    DWORD oldProtect;

    ChangedPageProtection (HANDLE process, LPVOID address, DWORD size, DWORD newProtect) {
        this->process = process;
        this->address = address;
        this->size = size;
        this->oldProtect = 0;
        
        if (!VirtualProtectEx(
            process, address, size, newProtect, &(this->oldProtect)
        )) {
            auto error = GetLastError();
            throw std::exception("Failed to change page protection");
        }
    }

    ~ChangedPageProtection () {
        if (this->process == NULL)
            throw;

        DWORD temp;
        auto rv = VirtualProtectEx(
            this->process, this->address, this->size, this->oldProtect, &temp
        );

        if (!rv) {
            auto error = GetLastError();
            return;
        } else {
            this->process = NULL;
            this->address = NULL;
            this->size = this->oldProtect = 0;
        }
    }
};

XPTRACE_EXPORT(bool) xptrace_get_marker_enabled_by_id (markerid id) {
    return markers[id].enabled;
}

XPTRACE_EXPORT(bool) xptrace_set_marker_enabled_by_id (markerid id, bool newState) {
    auto marker = &markers[id];
    if ((marker->enabled == newState) && (marker->initialized))
        return true;

    marker->enabled = newState;

    auto process = GetCurrentProcess();
    auto address = (reinterpret_cast<unsigned char *>(const_cast<void *>(marker->return_address)) - SIZEOF_CALL);

    {
        auto protection = ChangedPageProtection(
            GetCurrentProcess(), address, SIZEOF_CALL, PAGE_EXECUTE_READWRITE
        );

        if (!marker->initialized) {
            if (address[0] != xptrace::CALL)
                return false;

            memcpy(marker->original_bytes, address, SIZEOF_CALL);
            marker->initialized = true;
        }

        if (newState == false) {
            memset(address, xptrace::NOP, SIZEOF_CALL);
        } else { 
            memcpy(address, marker->original_bytes, SIZEOF_CALL);
        }
    }

    FlushInstructionCache(process, address, SIZEOF_CALL);

    return true;
}

XPTRACE_EXPORT(bool) xptrace_add_marker_callback_by_id (markerid id, marker_callback callback, void * userdata) {
    auto marker = &markers[id];

    callback_entry newentry = {
        callback, userdata
    };

    marker->callbacks.push_back(newentry);

    return true;
}

XPTRACE_EXPORT(void) xptrace_marker_hit (markerid id) {
    auto marker = markers[id];

    if (xptrace::logging_enabled) {
        if (marker.initialized) {
            if (marker.enabled)
                printf("marker hit: %x '%s'\n", id, marker.name);
            else
                printf("disabled marker hit: %x '%s'\n", id, marker.name);
        } else {
            printf("uninitialized marker hit: %x '%s'\n", id, marker.name);
        }
    }

    if (marker.initialized && marker.enabled) {
        for (auto iter = marker.callbacks.begin(); iter != marker.callbacks.end(); ++iter)
            iter->callback(id, iter->userdata);
    }

    return;
}

XPTRACE_EXPORT(bool) xptrace_set_logging_enabled (bool newState) {
    bool oldState = xptrace::logging_enabled;
    xptrace::logging_enabled = newState;
    return oldState;
}

XPTRACE_EXPORT(void) xptrace_enumerate_markers (marker_callback callback, void * userdata) {
    int size = markers.size();

    for (unsigned int i = 0; i < size; i++)
        callback(i, userdata);
}

XPTRACE_EXPORT(void) xptrace_shutdown () {
}