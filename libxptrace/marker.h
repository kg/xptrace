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
    struct __call_timer { \
        __pragma(strict_gs_check, push, off) \
        __pragma(check_stack, off) \
        __forceinline static void names (const char *& enter_name, const char *& exit_name) { \
            static bool initialized = false; \
            static const char * enter_name_storage, * exit_name_storage; \
            if (!initialized) { \
                initialized = true; \
                enter_name_storage = enter_name; \
                exit_name_storage = exit_name; \
            } else { \
                enter_name = enter_name_storage; \
                exit_name = exit_name_storage; \
            } \
        } \
        __declspec(noinline) static void enter () { \
            const char * enter_name, * exit_name; \
            __call_timer::names(enter_name, exit_name); \
            static const xptrace::markerid id = xptrace_register_marker(enter_name, _ReturnAddress()); \
            xptrace_marker_hit(id); \
        }; \
        __declspec(noinline) static void exit () { \
            const char * enter_name, * exit_name; \
            __call_timer::names(enter_name, exit_name); \
            static const xptrace::markerid id = xptrace_register_marker(exit_name, _ReturnAddress()); \
            xptrace_marker_hit(id); \
        }; \
        __forceinline __call_timer (const char * enter_name, const char * exit_name) { \
            __call_timer::names(enter_name, exit_name); \
            __call_timer::enter(); \
        }; \
        __forceinline ~__call_timer () { \
            __call_timer::exit(); \
        }; \
        __pragma(strict_gs_check, pop) \
        __pragma(check_stack) \
    } ___call_timer( \
        __FUNCTION__ "::enter", \
        __FUNCTION__ "::exit" \
    ); void
