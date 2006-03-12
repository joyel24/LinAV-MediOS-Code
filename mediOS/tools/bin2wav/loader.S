@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Loader for the Gmini400 folder info/wav file exploit
@
@ Date:     11/09/2005
@ Author:   GliGli
@

.text
.thumb

@ set UART0 to 115200 bauds
ldr r5,=0x30302
mov r1,#0x36
strh r1,[r5]

@
@ cache init
@
ldr r5,=0x30e00
@  disable cache
mov r1,#0x0
strh r1,[r5]
@  clear cache
mov r1,#0x2
strh r1,[r5]
cacheclear:
  ldrb r2,[r5]
  and r2,r1
  bne cacheclear
@  re-enable cache
ldr r1,=0x0101
strh r1,[r5]

@ go to kernel start
ldr r0,=0xc10000
mov lr,pc
bx r0

@ should never happen
loop:b loop
