/* 
*   gfxmemcpy.S
*
*   AMOS project
*
*/
		.section .core
/*
 * Prototype: void gfxmemcpy (void *to,const void *from,unsigned long n32pads);
 */
 

 
.global gfxmemcpy
gfxmemcpy:
	cmp r2, #0
	beq gfxmemcpy_exit

	stmdb sp!, {r4, r5}

	and r4, r2, #3
	sub r2, r2, r4
	cmp r4, #0
	beq gfxmemcpyfast
gfxmemcpy_loop:
	ldmia r1!, {r5}
	stmia r0!, {r5}
	subs r4, r4, #1
	bne gfxmemcpy_loop

gfxmemcpyfast:
	cmp r2, #0
	beq gfxmemcpyfast_exit

	stmdb sp!, {r6, r7, r8}
gfxmemcpyfast_loop:
	ldmia r1!, {r5, r6, r7, r8}
	stmia r0!, {r5, r6, r7, r8}
	subs r2, r2, #4
	bne gfxmemcpyfast_loop
	ldmia sp!, {r6, r7, r8}

gfxmemcpyfast_exit:
	ldmia sp!, {r4, r5}

gfxmemcpy_exit:
	mov pc, lr
