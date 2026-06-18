.section ".core"
.arm
.global convert_line

convert_line:
    stmfd sp!,{r0-r7,lr}

    mov r3,#64
    cvt_loop:
        ldrb r4,[r1],#1
        ldrb r5,[r1],#1
        ldrb r6,[r1],#1
        ldrb r7,[r1],#1

        ldr r4,[r2,r4,lsl #2]
        ldr r5,[r2,r5,lsl #2]
        ldr r6,[r2,r6,lsl #2]
        ldr r7,[r2,r7,lsl #2]

        stmia r0!,{r4,r5,r6,r7}

        subs r3,r3,#1
        bne cvt_loop

    ldmfd sp!,{r0-r7,pc}
