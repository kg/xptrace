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

#include "wildcard.h"

template <typename Callback> __forceinline void enumerate_markers_matching (const char * wildcard, Callback callback) {
    int size = markers.size();

    for (unsigned int i = 0; i < size; i++) {
        xptrace::marker& marker = markers[i];

        if (wildcmp(wildcard, marker.name.c_str()))
            callback(marker);
    }
}