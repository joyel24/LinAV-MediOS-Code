// avOS - http://avos.sourceforge.net
// Copyright (c) 2003 by Jimmy Moore
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.
//
// Date:     01/03/2004
// Author:   By Oxygen

#include "stdarg.h"
#include "string.h"

#include <uart.h>


static char debugmembuf[100];

void debug(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    va_end(ap);
    uartOutsA(debugmembuf);
}

char p[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxxxxxxxxx\n";

void printBuffer(char *source,int size) {
    int i,j;
    char c;

    for (j=0;j<size;j+=16) {
        for (i=0;i<16;i++) {
            c = source[i+j];
            stringPutHexA(p+(i*2), c, 2);
            if (c<32 || c>126) c='.';
            p[35+i] = c;
        }
        uartOutsA(p);
    }
	uartOutsA("\n\n");
}
