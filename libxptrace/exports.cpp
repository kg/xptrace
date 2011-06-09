#include "xptrace.h"
#include "statics.h"
#include "windows.h"

using namespace xptrace;

XPTRACE_EXPORT(void) xptrace_initialize () {
}

XPTRACE_EXPORT(marker::id) xptrace_register_marker (const char * name, const void * return_address) {
    marker::id newId = markers.size();
    marker newMarker = {
        string(name), return_address, true
    };
    markers.push_back(newMarker);

    xptrace_set_marker_enabled(newId, false);

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

XPTRACE_EXPORT(bool) xptrace_get_marker_enabled (marker::id id) {
    return markers[id].enabled;
}

XPTRACE_EXPORT(bool) xptrace_set_marker_enabled (marker::id id, bool newState) {
    auto marker = &markers[id];
    if (marker->enabled == newState)
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

XPTRACE_EXPORT(void) xptrace_marker_hit (marker::id id) {
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

    return;
}

XPTRACE_EXPORT(bool) xptrace_set_logging_enabled (bool newState) {
    bool oldState = xptrace::logging_enabled;
    xptrace::logging_enabled = newState;
    return oldState;
}

XPTRACE_EXPORT(void) xptrace_enumerate_markers (marker_enumerator callback, void * userdata) {
    int size = markers.size();

    for (unsigned int i = 0; i < size; i++)
        callback(i, userdata);
}

XPTRACE_EXPORT(void) xptrace_shutdown () {
}