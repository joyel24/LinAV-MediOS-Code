/*
*   lib/strchr.S
*
*   AvLo - linav project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
*   Code from uClinux
*   Copyright (C) 1995, 1996 Russell King
*
*  ASM optimised string functions
*/
    
              .text
              .align       5
.global strchr
strchr:
1:            
    ldrb      r2, [r0], #1
    teq       r2, r1
    teqne     r2, #0
    bne       1b
    teq       r2, #0
    moveq     r0, #0
    subne     r0, r0, #1
    mov       pc,lr
