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

int main (int argc, const char * argv[]) {
    setbuf(stdout, 0);
    setbuf(stderr, 0);

    if (argc != 2)
        return 1;

    switch (atoi(argv[1])) {
        case 0: {
            printf("$0\n");

            function_with_marker();

            getc(stdin);
            printf("$1\n");

            function_with_marker();

            getc(stdin);
            printf("$2\n");

            function_with_marker();

            getc(stdin);
            printf("$3\n");

            function_with_marker();

            printf("$4\n");

            return 0;
        }
    }
}