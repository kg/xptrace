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

#ifndef __XPTRACE_H__
#define __XPTRACE_H__

#include "common.h"

namespace xptrace {

    struct callback_entry;
    struct marker;

    // Size of call instruction (in bytes). Architecture dependent.
    const unsigned int SIZEOF_CALL = 5;
    // NOP instruction to replace a call instruction with. Must be a single byte.
    const unsigned char NOP = 0x90;
    // First byte of a call instruction (for sanity checks).
    const unsigned char CALL = 0xE8;

    typedef void (* marker_callback)(unsigned int id, void * userdata);

    typedef unsigned int markerid;

}

#ifdef XPTRACE_IMPL
#include "statics.h"
#endif

#include "marker.h"
#include "api.h"

#ifdef XPTRACE_IMPL
#include "inline.h"
#endif

#endif