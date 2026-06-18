/* 
*   include/kernel/io.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __IO_H
#define __IO_H

#define __real_inb(a)         (*(volatile unsigned char *)(a))
#define __real_inw(a)         (*(volatile unsigned short *)(a))
#define __real_inl(a)         (*(volatile unsigned int  *)(a))

#define __real_outb(v,a)      (*(volatile unsigned char *)(a) = (v))
#define __real_outw(v,a)      (*(volatile unsigned short *)(a) = (v))
#define __real_outl(v,a)      (*(volatile unsigned int  *)(a) = (v))

#define outb(v,p)     __real_outb(v,p)
#define outw(v,p)     __real_outw(v,p)
#define outl(v,p)     __real_outl(v,p)

#define inb(p)        ({ unsigned int __v = __real_inb(p); __v; })
#define inw(p)        ({ unsigned int __v = __real_inw(p); __v; })
#define inl(p)        ({ unsigned int __v = __real_inl(p); __v; })

#endif
