@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Date:     19/02/2004
@ Author:   By DoggerMoore
@
@

.text

.globl main2

main2:
    b main
    
    .space 0x4c, 0
    .asciz "CJBMVERSION2000"
    .space 0x20, 0

