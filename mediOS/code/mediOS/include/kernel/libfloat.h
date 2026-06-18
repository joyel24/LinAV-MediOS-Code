/*
*   include/kernel/libfloat.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __LIBFLOAT_H
#define __LIBFLOAT_H

long float_modsi3(long a, long b);
long float_umodsi3(long a, long b);
long float_divsi3(long a, long b);
long float_udivsi3(long a, long b);
unsigned int float_divdi3(unsigned int a, unsigned int b);

float float_addsf3 (float a, float b);
float float_subsf3 (float a, float b);
float float_mulsf3 (float a, float b);
float float_divsf3 (float a, float b);
double float_floatsidf (register long a);
double float_floatdidf (register long long a);
float float_floatsisf (register long  a);
float float_floatdisf (register long long  a);
float float_negsf2 (float a);
double float_negdf2 (double a);
double float_extendsfdf2 (float a);
float float_truncdfsf2 (double a);
long float_fixsfsi (float a);
long float_fixdfsi (double a);
long long float_fixdfdi (double a);
unsigned long float_fixunsdfsi (double a);
unsigned long long float_fixunsdfdi (double a);
double float_adddf3 (double a, double b);
double float_subdf3 (double a, double b);
double float_muldf3 (double a, double b);
double float_divdf3 (double a, double b);
int float_gtdf2 (double a, double b);
int float_gedf2 (double a, double b);
int float_ltdf2 (double a, double b);
int float_ledf2 (double a, double b);
int float_eqdf2 (double a, double b);
int float_nedf2 (double a, double b);
int float_gtsf2 (float a, float b);
int float_gesf2 (float a, float b);
int float_ltsf2 (float a, float b);
int float_lesf2 (float a, float b);
int float_eqsf2 (float a, float b);
int float_nesf2 (float a, float b);

long __modsi3(long, long);
long __umodsi3(long, long);
long __divsi3(long, long);
long __udivsi3(long, long);
unsigned int __divdi3(unsigned int, unsigned int);

float __addsf3 (float, float);
float __subsf3 (float, float);
float __mulsf3 (float, float);
float __divsf3 (float, float);
double __floatsidf (register long);
double __floatdidf (register long long);
float __floatsisf (register long );
float __floatdisf (register long long );
float __negsf2 (float);
double __negdf2 (double);
double __extendsfdf2 (float);
float __truncdfsf2 (double);
long __fixsfsi (float);
long __fixdfsi (double);
long long __fixdfdi (double);
unsigned long __fixunsdfsi (double);
unsigned long long __fixunsdfdi (double);
double __adddf3 (double, double);
double __subdf3 (double, double);
double __muldf3 (double, double);
double __divdf3 (double, double);
int __gtdf2 (double, double);
int __gedf2 (double, double);
int __ltdf2 (double, double);
int __ledf2 (double, double);
int __eqdf2 (double, double);
int __nedf2 (double, double);
int __gtsf2 (float, float);
int __gesf2 (float, float);
int __ltsf2 (float, float);
int __lesf2 (float, float);
int __eqsf2 (float, float);
int __nesf2 (float, float);


#endif
