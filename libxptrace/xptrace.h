#ifndef __XPTRACE_H__
#define __XPTRACE_H__

#include "common.h"
#include "string.h"

namespace xptrace {

    struct string;
    struct module;
    struct marker;

    typedef void (* module_initializer)();

    struct string {
        const char * characters;
        const size_t length;

        string (const char * null_terminated)
            : characters(null_terminated)
            , length(strlen(null_terminated)) {
        }

        int compareTo (const string& rhs) const;

        friend bool operator < (const string& lhs, const string& rhs);
        friend bool operator > (const string& lhs, const string& rhs);

        friend bool operator == (const string& lhs, const string& rhs);
        friend bool operator != (const string& lhs, const string& rhs);
    };

    struct module {
        string name;
        module_initializer * initializer;
        bool is_initialized;

        typedef unsigned int id;
    };

    struct marker {
        string name;
        const void * return_address;

        typedef unsigned int id;
    };

}

#include "api.h"
#include "marker.h"

#endif