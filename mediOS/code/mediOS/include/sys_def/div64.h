/* 
*   sys_def/div64.f
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* code from uClinux
*
*/

#ifndef _SYS_DEF_DIV64_H
#define _SYS_DEF_DIV64_H

#include <sys_def/types.h>

/*
 * do_div() performs a 64bit/32bit unsigned division and modulo.
 * The 64bit result is stored back in the divisor, the 32bit
 * remainder is returned.
 *
 * A semantically correct implementation would do this:
 *
 *	static inline uint32_t do_div(uint64_t &n, uint32_t base)
 *	{
 *		uint32_t rem;
 *		rem = n % base;
 *		n = n / base;
 *		return rem;
 *	}
 *
 * We can't rely on GCC's "long long" math since it would turn
 * everything into a full 64bit division implemented through _udivdi3(),
 * which is much slower.
 */

// BITS_PER_LONG == 32

# define do_div(n,base)	({					\
								\
	uint32_t __low, __low2, __high, __rem;			\
	__low  = (n) & 0xffffffff;				\
	__high = (n) >> 32;					\
	if (__high) {						\
		__rem   = __high % (uint32_t)(base);		\
		__high  = __high / (uint32_t)(base);		\
		__low2  = __low >> 16;				\
		__low2 += __rem << 16;				\
		__rem   = __low2 % (uint32_t)(base);		\
		__low2  = __low2 / (uint32_t)(base);		\
		__low   = __low & 0xffff;			\
		__low  += __rem << 16;				\
		__rem   = __low  % (uint32_t)(base);		\
		__low   = __low  / (uint32_t)(base);		\
		(n) = __low  + ((uint64_t)__low2 << 16) +	\
			((uint64_t) __high << 32);		\
	} else {						\
		__rem = __low % (uint32_t)(base);		\
		(n) = (__low / (uint32_t)(base));		\
	}							\
	__rem;							\
 })

#endif
