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

float float_addsf3 (float a, float b) { return __addsf3(a,b); }
float float_subsf3 (float a, float b) { return __subsf3(a,b); }
float float_mulsf3 (float a, float b) { return __mulsf3(a,b); }
float float_divsf3 (float a, float b) { return __divsf3(a,b); }
double float_floatsidf (register long a) { return __floatsidf(a); }
double float_floatdidf (register long long a) { return __floatdidf(a); }
float float_floatsisf (register long  a) { return __floatsisf(a); }
float float_floatdisf (register long long  a) { return __floatdisf(a); }
float float_negsf2 (float a) { return __negsf2(a); }
double float_negdf2 (double a) { return __negdf2(a); }
double float_extendsfdf2 (float a) { return __extendsfdf2(a); }
float float_truncdfsf2 (double a) { return __truncdfsf2(a); }
long float_fixsfsi (float a) { return __fixsfsi(a); }
long float_fixdfsi (double a) { return __fixdfsi(a); }
long long float_fixdfdi (double a) { return __fixdfdi(a); }
unsigned long float_fixunsdfsi (double a) { return __fixunsdfsi(a); }
unsigned long long float_fixunsdfdi (double a) { return __fixunsdfdi(a); }
double float_adddf3 (double a, double b) { return __adddf3(a,b); }
double float_subdf3 (double a, double b) { return __subdf3(a,b); }
double float_muldf3 (double a, double b) { return __muldf3(a,b); }
double float_divdf3 (double a, double b) { return __divdf3(a,b); }
int float_gtdf2 (double a, double b) { return __gtdf2(a,b); }
int float_gedf2 (double a, double b) { return __gedf2(a,b); }
int float_ltdf2 (double a, double b) { return __ltdf2(a,b); }
int float_ledf2 (double a, double b) { return __ledf2(a,b); }
int float_eqdf2 (double a, double b) { return __eqdf2(a,b); }
int float_nedf2 (double a, double b) { return __nedf2(a,b); }
int float_gtsf2 (float a, float b) { return __gtsf2(a,b); }
int float_gesf2 (float a, float b) { return __gesf2(a,b); }
int float_ltsf2 (float a, float b) { return __ltsf2(a,b); }
int float_lesf2 (float a, float b) { return __lesf2(a,b); }
int float_eqsf2 (float a, float b) { return __eqsf2(a,b); }
int float_nesf2 (float a, float b) { return __nesf2(a,b); }
