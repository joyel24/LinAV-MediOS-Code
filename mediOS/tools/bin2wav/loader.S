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

@ go to kernel start
ldr r0,=0xc10000
bx r0

@ should never happen
loop:b loop
