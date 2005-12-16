/*
*   include/irq.h
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __IRQ_H
#define __IRQ_H


#define __clf()                            \
    ({                            \
        unsigned long temp;                \
    __asm__ __volatile__(                    \
    "mrs    %0, cpsr        @ clf\n"        \
"    orr    %0, %0, #64\n"                    \
"    msr    cpsr_c, %0"                    \
    : "=r" (temp)                        \
    :                            \
    : "memory");                        \
    })

#define __cli()                         \
    ({                          \
        unsigned long temp;             \
    __asm__ __volatile__(                   \
    "mrs    %0, cpsr        @ cli\n"        \
"   orr %0, %0, #128\n"                 \
"   msr cpsr_c, %0"                 \
    : "=r" (temp)                       \
    :                           \
    : "memory");                        \
    })


#endif
