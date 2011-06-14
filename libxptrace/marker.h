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

#define XPTRACE_MARKER_2(name, subname) \
    struct name { \
        __pragma(strict_gs_check, push, off) \
        __pragma(check_stack, off) \
        __declspec(noinline) static void __fastcall subname () { \
            static const xptrace::markerid id = xptrace_register_marker(__FUNCTION__, _ReturnAddress()); \
            xptrace_marker_hit(id); \
        }; \
        __pragma(strict_gs_check, pop) \
        __pragma(check_stack) \
    }; \
    name::subname()

#define XPTRACE_MARKER(name) XPTRACE_MARKER_2(name, hit)

#define XPTRACE_TIME_FUNCTION \
    __pragma(strict_gs_check, push, off) \
    __pragma(check_stack, off) \
    static const char * __enter = __FUNCTION__ "::enter"; \
    static const char * __exit = __FUNCTION__ "::exit"; \
    struct call_timer { \
        __declspec(noinline) call_timer::call_timer () { \
            static const xptrace::markerid id = xptrace_register_marker(__enter, _ReturnAddress()); \
            xptrace_marker_hit(id); \
        } \
        __declspec(noinline) call_timer::~call_timer () { \
            static const xptrace::markerid id = xptrace_register_marker(__exit, _ReturnAddress()); \
            xptrace_marker_hit(id); \
        } \
    } _call_timer; \
    __pragma(strict_gs_check, pop) \
    __pragma(check_stack, restore) \
    void

