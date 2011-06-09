#include "xptrace.h"
#include "stdio.h"

void function_with_marker () {
    XPTRACE_MARKER(marker);
}

void marker_callback (xptrace::markerid id, void * userdata) {
    printf("marker_callback(%x, %x)\n", id, userdata);
}

void set_marker_enabled (xptrace::markerid id, void * newState) {
    xptrace_set_marker_enabled_by_id(id, reinterpret_cast<bool>(newState));
}

void add_marker_callback (xptrace::markerid id, void * userdata) {
    xptrace_add_marker_callback_by_id(id, marker_callback, userdata);
}

int main () {
    printf("0\n");

    function_with_marker();

    printf("1\n");

    xptrace_enumerate_markers(add_marker_callback, reinterpret_cast<void *>(1));

    function_with_marker();

    printf("2\n");

    xptrace_enumerate_markers(set_marker_enabled, reinterpret_cast<void *>(true));

    function_with_marker();

    printf("3\n");

    xptrace_enumerate_markers(add_marker_callback, reinterpret_cast<void *>(2));

    function_with_marker();

    printf("4\n");
}