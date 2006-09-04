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
float float_floatsisf(long a);
float float_divsf3 (float a, float b);
float float_addsf3 (float a, float b);
float float_mulsf3 (float a, float b);
float float_subsf3 (float a, float b);
int float_fixsfsi (float a1);
int float_nesf2(float a, float b);

long __modsi3(long a, long b);
long __umodsi3(long a, long b);
long __divsi3(long a, long b);
long __udivsi3(long a, long b);
unsigned int __divdi3(unsigned int a, unsigned int b);
float __floatsisf(long a);
float __divsf3 (float a, float b);
float __addsf3 (float a, float b);
float __mulsf3 (float a, float b);
float __subsf3 (float a, float b);
int __fixsfsi (float a1);
int __nesf2(float a, float b);
#endif
