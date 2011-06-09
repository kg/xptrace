extern "C" {
    void * _ReturnAddress(void);
    #pragma intrinsic(_ReturnAddress)
}

XPTRACE_EXPORT(xptrace::markerid) xptrace_register_marker (const char * name, const void * return_address);
XPTRACE_EXPORT(void) xptrace_marker_hit (xptrace::markerid id);
XPTRACE_EXPORT(bool) xptrace_set_logging_enabled (bool newState);
XPTRACE_EXPORT(bool) xptrace_get_marker_enabled_by_id (xptrace::markerid id);
XPTRACE_EXPORT(bool) xptrace_set_marker_enabled_by_id (xptrace::markerid id, bool newState);
XPTRACE_EXPORT(bool) xptrace_add_marker_callback_by_id (xptrace::markerid id, xptrace::marker_callback callback, void * userdata);
XPTRACE_EXPORT(void) xptrace_enumerate_markers (xptrace::marker_callback callback, void * userdata);