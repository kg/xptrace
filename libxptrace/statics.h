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

#include <map>
#include <vector>
#include <list>

namespace xptrace {

    struct callback_entry {
        marker_callback callback;
        void * userdata;
    };

    struct marker {
        markerid id;
        string name;
        const void * return_address;
        bool enabled, initialized;
        unsigned char original_bytes[SIZEOF_CALL];
        std::vector<callback_entry> callbacks;
    };

    struct callback_wildcard {
        string wildcard;
        marker_callback callback;
        void * userdata;
    };

    struct enabled_wildcard {
        string wildcard;
        bool enabled;
    };

    extern std::vector<marker> markers;
    extern std::list<callback_wildcard> callback_wildcards;
    extern std::list<enabled_wildcard> enabled_wildcards;

    extern bool logging_enabled;

}