.ifndef utilInc
utilInc = 1

.text
.thumb

@@@@@@@@@@@@@@@@@@@@@@@
@ codecpy
@ r0->buffer r1->offset r2->size)
@
.globl codecpy
.thumb_func
codecpy:
	push {r0, r1, r2, r3}
	ldr r3, =ramStart
	add r1, r3
loopCpy:
	ldr r3, [r0]
	str r3, [r1]
	add r0, #4
	add r1, #4
	sub r2, #4
	beq stopCpy
	b loopCpy
stopCpy:
	pop {r0, r1, r2, r3}
	bx lr

@@@@@@@@@@@@@@@@@@@@@@@
@ startProg
@ r0 arg count, r1 argv (char **)
@
.globl startProg
.thumb_func
startProg:
	push {r2, lr}
	mov r2, #1
	mov lr, r2
	ldr r2, =ramStart
	add lr, pc
	bx r2
	pop {r2, pc}

.endif
