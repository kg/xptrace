#ifdef XPTRACE_IMPL
    #define XPTRACE_EXPORT_2(return_type, callconv) extern "C" __declspec(dllexport) return_type callconv
#else
    #define XPTRACE_EXPORT_2(return_type, callconv) extern "C" __declspec(dllimport) return_type callconv
#endif

#define XPTRACE_EXPORT(return_type) XPTRACE_EXPORT_2(return_type, __stdcall)