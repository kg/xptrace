#include "xptrace.h"
#include "statics.h"

using namespace xptrace;

XPTRACE_EXPORT(void) xptrace_initialize () {
}

XPTRACE_EXPORT(module::id) xptrace_get_module (const char * name) {
    auto _name = string(name);
    auto existing = modules_by_name.find(_name);
    if (existing != modules_by_name.end())
        return existing->second;

    return -1;
}

XPTRACE_EXPORT(module::id) xptrace_register_module (const char * name, module_initializer * initializer) {
    auto _name = string(name);
    auto existing = modules_by_name.find(_name);
    if (existing != modules_by_name.end())
        return existing->second;

    module::id newId = modules.size();
    module newModule = { 
        _name, initializer, false
    };
    modules.push_back(newModule);
    modules_by_name[name] = newId;

    return newId;
}

XPTRACE_EXPORT(marker::id) xptrace_register_marker (const char * name, const void * return_address) {
    marker::id newId = markers.size();
    marker newMarker = {
        string(name), return_address
    };
    markers.push_back(newMarker);

    return newId;
}

XPTRACE_EXPORT(void) xptrace_marker_hit (marker::id id) {
    auto marker = markers.at(id);

    if (xptrace::logging_enabled)
        printf("marker hit: %x '%s'\n", id, marker.name);

    return;
}

XPTRACE_EXPORT(bool) xptrace_set_logging_enabled (bool newState) {
    bool oldState = xptrace::logging_enabled;
    xptrace::logging_enabled = newState;
    return oldState;
}

XPTRACE_EXPORT(void) xptrace_shutdown () {
}