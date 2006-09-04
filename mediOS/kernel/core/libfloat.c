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
float float_floatsisf(long a) { return __floatsisf(a);}
float float_divsf3 (float a, float b) { return __divsf3(a,b); }
float float_addsf3 (float a, float b) { return __addsf3(a,b); }
float float_mulsf3 (float a, float b) { return __mulsf3(a,b); }
float float_subsf3 (float a, float b) { return __subsf3(a,b); }
int float_fixsfsi (float a) { return __fixsfsi(a); }
int float_nesf2(float a, float b) {return __nesf2(a,b);}