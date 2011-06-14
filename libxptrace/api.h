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

extern "C" {
    void * _ReturnAddress(void);
    #pragma intrinsic(_ReturnAddress)
}

XPTRACE_EXPORT(xptrace::markerid) xptrace_register_marker (const char * name, const void * return_address);
XPTRACE_EXPORT(void) xptrace_marker_hit (xptrace::markerid id);

XPTRACE_EXPORT(bool) xptrace_set_logging_enabled (bool newState);

XPTRACE_EXPORT(bool) xptrace_get_marker_enabled_by_id (xptrace::markerid id);
XPTRACE_EXPORT(bool) xptrace_set_marker_enabled_by_id (xptrace::markerid id, bool newState);
XPTRACE_EXPORT(void) xptrace_add_marker_callback_by_id (xptrace::markerid id, xptrace::marker_callback callback, void * userdata);
XPTRACE_EXPORT(void) xptrace_remove_marker_callback_by_id (xptrace::markerid id, xptrace::marker_callback callback, void * userdata);

XPTRACE_EXPORT(void) xptrace_enumerate_markers (xptrace::marker_callback callback, void * userdata);

XPTRACE_EXPORT(void) xptrace_set_markers_enabled (const char * wildcard, bool newState);
XPTRACE_EXPORT(void) xptrace_add_markers_callback (const char * wildcard, xptrace::marker_callback callback, void * userdata);
XPTRACE_EXPORT(void) xptrace_remove_markers_callback (const char * wildcard, xptrace::marker_callback callback, void * userdata);
