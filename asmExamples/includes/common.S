@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Common - General purpose macros, equates, etc
@
@
@ Date:     09/01/2004
@ Author:   By DoggerMoore
@

.macro  callThumb   tempReg, funName
    mov \tempReg, #1
    mov lr, \tempReg
    ldr \tempReg, =(\funName+1)
    add lr, pc
    bx \tempReg
.endm

.macro callThumb1    tempReg, funName, arg0
    ldr r0, =\arg0
    mov \tempReg, #1
    mov lr, \tempReg
    ldr \tempReg, =(\funName+1)
    add lr, pc
    bx \tempReg
.endm

.macro callThumb2    tempReg, funName, arg0, arg1
    ldr r0, =\arg0
    ldr r1, =\arg1
    mov \tempReg, #1
    mov lr, \tempReg
    ldr \tempReg, =(\funName+1)
    add lr, pc
    bx \tempReg
.endm

