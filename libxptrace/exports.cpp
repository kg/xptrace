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
#include <algorithm>

using namespace xptrace;

XPTRACE_EXPORT(void) xptrace_initialize () {
}

XPTRACE_EXPORT(markerid) xptrace_register_marker (const char * name) {
    bool enabledState = false;

    for (auto iter = enabled_wildcards.begin(), end = enabled_wildcards.end(); iter != end; ++iter) {
        if (wildcmp(iter->wildcard.c_str(), name))
            enabledState = iter->enabled;
    }

    markerid newId = markers.size();
    marker newMarker = {
        newId, std::string(name), enabledState
    };
    markers.push_back(newMarker);

    for (auto iter = callback_wildcards.begin(), end = callback_wildcards.end(); iter != end; ++iter) {
        if (wildcmp(iter->wildcard.c_str(), name))
            xptrace_add_marker_callback_by_id(newId, iter->callback, iter->userdata);
    }

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

static void set_callsite_enabled (void * address, callsite_info callsite, bool newState) {
    auto process = GetCurrentProcess();

    {
        auto protection = ChangedPageProtection(
            GetCurrentProcess(), address, SIZEOF_CALL, PAGE_EXECUTE_READWRITE
        );

        if (newState == false) {
            memset(address, xptrace::NOP, SIZEOF_CALL);
        } else { 
            memcpy(address, callsite.original_bytes, SIZEOF_CALL);
        }
    }

    FlushInstructionCache(process, address, SIZEOF_CALL);
}

XPTRACE_EXPORT(bool) xptrace_set_marker_enabled_by_id (markerid id, bool newState) {
    marker& marker = markers[id];
    if (marker.enabled == newState)
        return true;

    marker.enabled = newState;

    for (auto iter = marker.callsites.begin(), end = marker.callsites.end(); iter != end; ++iter) {
        auto address = reinterpret_cast<unsigned char *>(
            const_cast<void *>(iter->first)
        ) - SIZEOF_CALL;

        set_callsite_enabled(address, iter->second, newState);
    }

    return true;
}

XPTRACE_EXPORT(void) xptrace_add_marker_callback_by_id (markerid id, marker_callback callback, void * userdata) {
    marker& marker = markers[id];

    callback_entry newentry = {
        callback, userdata
    };

    marker.callbacks.push_back(newentry);
}

XPTRACE_EXPORT(void) xptrace_remove_marker_callback_by_id (markerid id, marker_callback callback, void * userdata) {
    marker& marker = markers[id];

    callback_entry entry = {
        callback, userdata
    };

    auto erase_from = std::remove(marker.callbacks.begin(), marker.callbacks.end(), entry);
    marker.callbacks.erase(erase_from, marker.callbacks.end());
}

XPTRACE_EXPORT(void) xptrace_set_markers_enabled (const char * wildcard, bool newState) {
    struct setter {
        bool enabled;

        setter (bool enabled) {
            this->enabled = enabled;
        }

        void operator () (marker& marker) {
            xptrace_set_marker_enabled_by_id(marker.id, enabled);
        }
    };

    enumerate_markers_matching(wildcard, setter(newState));

    enabled_wildcard wc = {
        std::string(wildcard), newState
    };
    enabled_wildcards.push_back(wc);
}

XPTRACE_EXPORT(void) xptrace_add_markers_callback (const char * wildcard, xptrace::marker_callback callback, void * userdata) {
    struct adder {
        xptrace::marker_callback callback;
        void * userdata;

        adder (xptrace::marker_callback callback, void * userdata) {
            this->callback = callback;
            this->userdata = userdata;
        }

        void operator () (marker& marker) {
            callback_entry entry = {
                callback, userdata
            };
            marker.callbacks.push_back(entry);
        }
    };

    enumerate_markers_matching(wildcard, adder(callback, userdata));

    callback_wildcard wc = {
        std::string(wildcard),
        callback, userdata
    };
    callback_wildcards.push_back(wc);
}

XPTRACE_EXPORT(void) xptrace_remove_markers_callback (const char * wildcard, xptrace::marker_callback callback, void * userdata) {
    struct remover {
        xptrace::marker_callback callback;
        void * userdata;

        remover (xptrace::marker_callback callback, void * userdata) {
            this->callback = callback;
            this->userdata = userdata;
        }

        void operator () (marker& marker) {
            xptrace_remove_marker_callback_by_id(marker.id, this->callback, this->userdata);
        }
    };

    enumerate_markers_matching(wildcard, remover(callback, userdata));

    callback_wildcard wc = {
        std::string(wildcard),
        callback, userdata
    };
    auto erase_from = std::remove(callback_wildcards.begin(), callback_wildcards.end(), wc);
    callback_wildcards.erase(erase_from, callback_wildcards.end());
}

XPTRACE_EXPORT(void) xptrace_marker_hit (markerid id, void * returnAddress) {
    marker& marker = markers[id];

    auto callsite = marker.callsites.find(returnAddress);
    if (callsite == marker.callsites.end()) {
        auto address = reinterpret_cast<unsigned char *>(returnAddress) - SIZEOF_CALL;
        callsite_info info;
        memcpy(info.original_bytes, address, SIZEOF_CALL);
        marker.callsites[returnAddress] = info;

        set_callsite_enabled(address, info, marker.enabled);
    }

    if (xptrace::logging_enabled) {
        if (marker.enabled)
            printf("marker hit: %x '%s'\n", id, marker.name.c_str());
        else
            printf("disabled marker hit: %x '%s'\n", id, marker.name.c_str());
    }

    if (marker.enabled) {
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