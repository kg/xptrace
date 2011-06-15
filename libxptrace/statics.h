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
#include <string>

namespace xptrace {

    struct callback_entry {
        marker_callback callback;
        void * userdata;
    };

    static inline bool operator == (callback_entry lhs, callback_entry rhs) {
        return (lhs.callback == rhs.callback) &&
            (lhs.userdata == rhs.userdata);
    }

    struct callsite_info {
        unsigned char original_bytes[SIZEOF_CALL];
    };

    struct marker {
        markerid id;
        std::string name;
        bool enabled;
        std::map<const void *, callsite_info> callsites;
        std::vector<callback_entry> callbacks;
    };

    struct callback_wildcard {
        std::string wildcard;
        marker_callback callback;
        void * userdata;

        inline void operator = (const callback_wildcard& rhs) {
            wildcard = rhs.wildcard;
            callback = rhs.callback;
            userdata = rhs.userdata;
        }
    };

    static inline bool operator == (callback_wildcard lhs, callback_wildcard rhs) {
        return (lhs.wildcard == rhs.wildcard) && 
            (lhs.callback == rhs.callback) &&
            (lhs.userdata == rhs.userdata);
    }

    struct enabled_wildcard {
        std::string wildcard;
        bool enabled;
    };

    extern std::vector<marker> markers;
    extern std::list<callback_wildcard> callback_wildcards;
    extern std::list<enabled_wildcard> enabled_wildcards;

    extern bool logging_enabled;

}