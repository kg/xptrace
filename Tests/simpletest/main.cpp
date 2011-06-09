#include "xptrace.h"
#include "stdio.h"

int main () {
    printf("0\n");

    XPTRACE_MARKER(marker1);

    printf("1\n");

    XPTRACE_MARKER(marker2);

    printf("2\n");
}