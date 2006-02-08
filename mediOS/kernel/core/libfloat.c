/*
*   kernel/core/libfloat.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/libfloat.h>

long float_modsi3(long a, long b) { return __modsi3(a,b); }
long float_umodsi3(long a, long b) { return __umodsi3(a,b); }
long float_divsi3(long a, long b) { return __divsi3(a,b); }
long float_udivsi3(long a, long b) { return __udivsi3(a,b); }
unsigned int float_divdi3(unsigned int a, unsigned int b) { return __divdi3(a,b); }

