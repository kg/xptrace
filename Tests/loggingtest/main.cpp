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
#include "stdio.h"

void function_with_marker () {
    XPTRACE_MARKER(marker);
}

void set_marker_enabled (xptrace::markerid id, void * newState) {
    xptrace_set_marker_enabled_by_id(id, reinterpret_cast<bool>(newState));
}

int main () {
    xptrace_set_logging_enabled(true);

    printf("0\n");

    function_with_marker();

    printf("1\n");

    function_with_marker();

    printf("2\n");

    xptrace_enumerate_markers(set_marker_enabled, reinterpret_cast<void *>(true));
    function_with_marker();

    printf("3\n");

    xptrace_enumerate_markers(set_marker_enabled, reinterpret_cast<void *>(false));
    function_with_marker();

    printf("4\n");
}