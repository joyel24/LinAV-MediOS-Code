#include <uart.h>
#include <stdarg.h>
#include <string.h>

static char debugmembuf[100];

void debug(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    va_end(ap);
    uartOutsA(debugmembuf);
}
