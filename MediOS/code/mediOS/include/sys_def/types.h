/* 
*   include/sys_def/types.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*/

#ifndef __SYS_DEF_TYPES_H
#define __SYS_DEF_TYPES_H

#define    false FALSE
#define    true  TRUE

#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)

typedef short                int16_t;
typedef unsigned short       uint16_t;

typedef unsigned int         size_t;
typedef signed   int         ssize_t;

typedef unsigned int         mode_t;
typedef signed long          off_t;

typedef int                  int32_t;
typedef unsigned int         uint32_t;

typedef long long            int64_t;
typedef unsigned long long   uint64_t;

#ifndef __cplusplus
typedef enum {FALSE=0,TRUE=1}    bool;
#endif

struct pt_regs {
	long uregs[18];
};

#endif
