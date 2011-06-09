#define XPTRACE_MARKER(name) \
    struct name { \
        __pragma(strict_gs_check, push, off) \
        __pragma(check_stack, off) \
        __declspec(noinline) static void __fastcall hit () { \
            static const xptrace::marker::id id = xptrace_register_marker(__FUNCTION__, _ReturnAddress()); \
            xptrace_marker_hit(id); \
        }; \
        __pragma(strict_gs_check, pop) \
        __pragma(check_stack) \
    }; \
    name::hit(); 