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
#include "stdlib.h"

void function_with_marker () {
    XPTRACE_MARKER(marker);
}

void timed_function (int x) {
    XPTRACE_TIME_FUNCTION();

    printf("timed_function(%d)\n", x);

    if (x > 1)
        timed_function(x - 1);
}

void set_marker_enabled (xptrace::markerid id, void * newState) {
    xptrace_set_marker_enabled_by_id(id, reinterpret_cast<bool>(newState));
}

int main (int argc, const char * argv[]) {
    if (argc != 2)
        return 1;

    switch (atoi(argv[1])) {
        case 0: {
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

            return 0;
        }
        case 1: {
            xptrace_set_logging_enabled(true);
            xptrace_set_markers_enabled("*", true);

            printf("0\n");

            timed_function(1);

            printf("1\n");

            timed_function(2);

            printf("2\n");

            return 0;
        }
    }
}