@ avOS - http://avos.sourceforge.net
@ Copyright (c) 2003 by Jimmy Moore
@
@ All files in this archive are subject to the GNU General Public License.
@ See the file COPYING in the source tree root for full license agreement.
@ This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
@ KIND, either express of implied.
@
@ Timers
@
@ Date:     21/02/2004
@ Author:   By DoggerMoore
@
@

.ifndef timersInc
timersInc = 1

        TIMER0_BASE     =   0x30000
        TIMER1_BASE     =   0x30080
        TIMER2_BASE     =   0x30100
        TIMER3_BASE     =   0x30180

        TIMER_TMMD      =   0x00            @ Timer mode
        TIMER_TMCLK     =   0x02            @ Timer clock select
        TIMER_TMPS      =   0x04            @ Timer prescaler divide value
        TIMER_TMVAL     =   0x06            @ Timer maximum counter value
        TIMER_TMTRG     =   0x08            @ Timer trigger
                                            @ When set to oneshot, it starts
                                            @ when a 1 is written here.
        TIMER_TMCNT     =   0x0a            @ Timer current count value

        TIMER_TMMD_STOP     = 0x00
        TIMER_TMMD_ONESHOT  = 0x01          @ one shot mode
        TIMER_TMMD_FREERUN  = 0x02          @ continuous mode
        
        TIMER_TMCLK_ARM     = 0x00          @ Current ARM operation clock
        TIMER_TMCLK_EXT     = 0x01          @ External MXO/MXI or SYSCLK input

.text
  
@ ------------------------------------------------------------------------------
@ timersConfig(r0=timer, r1=config, r2=clk, r3=divval, sp=max val)
@
.globl timersConfig
.thumb_func

timersConfig:
        push {r0, r4, r5}
        ldr r4, [sp, #12]
        lsl r0, #7
        ldr r5, =0x30000
        add r5, r0
        
        strh r1, [r5, #TIMER_TMMD]           @ Store config
        strh r2, [r5, #TIMER_TMCLK]          @ Store clk select
        strh r3, [r5, #TIMER_TMPS]           @ Store prescaler divide value
        strh r4, [r5, #TIMER_TMVAL]          @ Store max value
        pop {r0, r4, r5}
        bx lr

@ ------------------------------------------------------------------------------
@ timersTrigger(r0=timer)
@
.globl timersTrigger
.thumb_func

timersTrigger:
        push {r0, r4}
        lsl r0, #7
        ldr r4, =0x30000
        add r4, r0
        mov r0, #1
        strh r0, [r5, #TIMER_TMTRG]          @ Trigger
        pop {r0, r4}
        bx lr

@ ------------------------------------------------------------------------------
@ timersGetValue(r0=timer)
@
.globl timersGetValue
.thumb_func

timersGetValue:
        push {r4}
        lsl r0, #7
        ldr r4, =0x30000
        add r4, r0
        ldrh r0, [r4, #TIMER_TMCNT]          @ Count
        pop {r4}
        bx lr
        
        .arm
        .ltorg

.endif
