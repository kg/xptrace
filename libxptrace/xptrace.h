#ifndef __XPTRACE_H__
#define __XPTRACE_H__

#include "common.h"
#include "string.h"

namespace xptrace {

    struct string;
    struct callback_entry;
    struct marker;

    // Size of call instruction (in bytes). Architecture dependent.
    const unsigned int SIZEOF_CALL = 5;
    // NOP instruction to replace a call instruction with. Must be a single byte.
    const unsigned char NOP = 0x90;
    // First byte of a call instruction (for sanity checks).
    const unsigned char CALL = 0xE8;

    typedef void (* string_destructor)(const string& s);
    typedef void (* marker_callback)(unsigned int id, void * userdata);

    typedef unsigned int markerid;

    struct string {
        const char * characters;
        const size_t length;
        const string_destructor destructor;

        string (const char * null_terminated)
            : characters(null_terminated)
            , length(strlen(null_terminated))
            , destructor(NULL) {
        }

        ~string () {
            if (destructor != NULL)
                destructor(*this);
        }

        int compareTo (const string& rhs) const;

        friend bool operator < (const string& lhs, const string& rhs);
        friend bool operator > (const string& lhs, const string& rhs);

        friend bool operator == (const string& lhs, const string& rhs);
        friend bool operator != (const string& lhs, const string& rhs);
    };

}

#ifdef XPTRACE_IMPL
#include "statics.h"
#endif

#include "marker.h"
#include "api.h"

#endif