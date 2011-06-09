#include "xptrace.h"
#include "stdio.h"

void function_with_marker () {
    XPTRACE_MARKER(marker);
}

void set_marker_enabled (xptrace::markerid id, void * newState) {
    xptrace_set_marker_enabled_by_id(id, reinterpret_cast<bool>(newState));
}

int main () {
    xptrace_set_logging_enabled(true);

    printf("0\n");

    function_with_marker();

    printf("1\n");

    function_with_marker();

    printf("2\n");

    xptrace_enumerate_markers(set_marker_enabled, reinterpret_cast<void *>(true));
    function_with_marker();

    printf("3\n");

    xptrace_enumerate_markers(set_marker_enabled, reinterpret_cast<void *>(false));
    function_with_marker();

    printf("4\n");
}