/*
 *  linux/arch/arm/kernel/head-armv.S
 *
 *  Copyright (C) 1994-1999 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  32-bit kernel startup code for all architectures
 *
 * added S3C4510 code by Mac Wang
 * added S3C4530 code by Arcturus Networks Inc.
 * added S3C2500 code by Arcturus Networks Inc.
 * added TI TMS320DM270 code by Chee Tim Loh <cheetim_loh@innomedia.com.sg>.
 *
 */
#include <linux/config.h>
#include <linux/linkage.h>

#include <asm/assembler.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

#include <asm/arch/hardware.h>

#define K(a,b,c)	((a) << 24 | (b) << 12 | (c))

/*
 * We place the page tables 16K below TEXTADDR.  Therefore, we must make sure
 * that TEXTADDR is correctly set.  Currently, we expect the least significant
 * "short" to be 0x8000, but we could probably relax this restriction to
 * TEXTADDR > PAGE_OFFSET + 0x4000
 *
 * Note that swapper_pg_dir is the virtual address of the page tables, and
 * pgtbl gives us a position-independent reference to these tables.  We can
 * do this because stext == TEXTADDR
 *
 * swapper_pg_dir, pgtbl and krnladr are all closely related.
 */

#ifndef CONFIG_UCLINUX
		.globl	SYMBOL_NAME(swapper_pg_dir)
		.equ	SYMBOL_NAME(swapper_pg_dir), TEXTADDR - 0x4000
		.macro	pgtbl, reg, rambase
		adr	\reg, stext
		sub	\reg, \reg, #0x4000
		.endm

/*
 * Since the page table is closely related to the kernel start address, we
 * can convert the page table base address to the base address of the section
 * containing both.
 */
		.macro	krnladr, rd, pgtable, rambase
		bic	\rd, \pgtable, #0x000ff000
		.endm
#endif

/*
 *  Kernel startup entry point.
 *
 * The rules are:
 *  r0      - should be 0
 *  r1      - unique architecture number
 *  MMU     - off
 *  I-cache - on or off
 *  D-cache - off
 *
 * See linux/arch/arm/tools/mach-types for the complete list of numbers
 * for r1.
 */


.section ".text.init",#alloc,#execinstr
		.type	stext, #function
ENTRY(stext)
				bl	__greet

				mov	r12, r0
/*
 * NOTE!  Any code which is placed here should be done for one of
 * the following reasons:
 *
 *  1. Compatability with old production boot firmware (ie, users
 *     actually have and are booting the kernel with the old firmware)
 *     and therefore will be eventually removed.
 *  2. Cover the case when there is no boot firmware.  This is not
 *     ideal, but in this case, it should ONLY set r0 and r1 to the
 *     appropriate value.
 */
	/* modified by oxygen77 */
	 @ make sure svc mode
	 @ and all irqs disabled
#ifdef CONFIG_ARCH_AV3XX
		mov	r0, #I_BIT | SVC_MODE
		msr cpsr_c, r0
#else
		mov	r0, #F_BIT | I_BIT | SVC_MODE
		msr cpsr_c, r0
#endif
	/**************************/

/* ////////////////////////////////////////////////////////////// */


		bl	__lookup_processor_type
		teq	r10, #0				@ invalid processor?
		moveq	r0, #'p'			@ yes, error 'p'
		beq	__error
		bl	__lookup_architecture_type

#ifndef CONFIG_UCLINUX
		bl	__create_page_tables
#endif
		adr	lr, __ret			@ return address
		add	pc, r10, #12			@ initialise processor
							@ (return control reg)

__switch_data:	.long	__mmap_switched
		.long	SYMBOL_NAME(compat) 					@r2
		.long	SYMBOL_NAME(__bss_start) 				@r4
		.long	SYMBOL_NAME(_end) 						@r5
		.long	SYMBOL_NAME(processor_id)				@r6
		.long	SYMBOL_NAME(__machine_arch_type)		@r7
		.long	SYMBOL_NAME(cr_alignment)				@r8
		.long	SYMBOL_NAME(_edata)						@r10
		.long	SYMBOL_NAME(init_task_union)+8192		@sp

__ret:		ldr	lr, __switch_data
#ifdef CONFIG_CPU_WITH_CACHE
# ifdef CONFIG_CPU_WITH_MCR_INSTRUCTION
		mcr	p15, 0, r0, c1, c0
		mov	r0, r0
		mov	r0, r0
		mov	r0, r0
# else
#  warning 	"FIXME: Enable Cache, Other settings without MCR Instruction"
		@ Note r0 is initialized suitably to enable Cache
		@ and additional settings if any in proc_armX.S file
		@ A possible code here
		@ldr	r2, CACHE_CONTROL_MEM_LOCATION
		@str	r0, [r2]
# endif
#endif
		mov	pc, lr

		/*
		 * This code follows on after the page
		 * table switch and jump above.
		 *
		 * r0  = processor control register
		 * r1  = machine ID
		 * r9  = processor ID
		 */
		.align	5
__mmap_switched:

		adr	r3, __switch_data + 4
		ldmia	r3, {r2, r4, r5, r6, r7, r8, r10, sp}@ r2 = compat
							@ sp = stack pointer

		str	r12, [r2]

/* added by oxygen77 */

/*			mov r11, r5
		lav3xx:
			cmp	r10, r5
			ldrcc	r12, [r10],#4
			strcc	r12, [r11],#4
			bcc	lav3xx*/
/*********************/
		ldr	r12, [r2]

		mov	fp, #0				@ Clear BSS (and zero fp)
1:		cmp	r4, r5
		strcc	fp, [r4],#4
		bcc	1b

		str	r9, [r6]			@ Save processor ID
		str	r1, [r7]			@ Save machine type

#ifdef CONFIG_ALIGNMENT_TRAP
		orr	r0, r0, #2			@ ...........A.
#endif
		bic	r2, r0, #2			@ Clear 'A' bit
		stmia	r8, {r0, r2}			@ Save control register values
		b	SYMBOL_NAME(start_kernel)


#ifndef CONFIG_UCLINUX
/*
 * Setup the initial page tables.  We only setup the barest
 * amount which are required to get the kernel running, which
 * generally means mapping in the kernel code.
 *
 * We only map in 4MB of RAM, which should be sufficient in
 * all cases.
 *
 * r5 = physical address of start of RAM
 * r6 = physical IO address
 * r7 = byte offset into page tables for IO
 * r8 = page table flags
 */
__create_page_tables:
		pgtbl	r4, r5				@ page table address

		/*
		 * Clear the 16K level 1 swapper page table
		 */
		mov	r0, r4
		mov	r3, #0
		add	r2, r0, #0x4000
1:		str	r3, [r0], #4
		str	r3, [r0], #4
		str	r3, [r0], #4
		str	r3, [r0], #4
		teq	r0, r2
		bne	1b

		/*
		 * Create identity mapping for first MB of kernel to
		 * cater for the MMU enable.  This identity mapping
		 * will be removed by paging_init()
		 */
		krnladr	r2, r4, r5			@ start of kernel
		add	r3, r8, r2			@ flags + kernel base
		str	r3, [r4, r2, lsr #18]		@ identity mapping

		/*
		 * Now setup the pagetables for our kernel direct
		 * mapped region.  We round TEXTADDR down to the
		 * nearest megabyte boundary.
		 */
		add	r0, r4, #(TEXTADDR & 0xff000000) >> 18 @ start of kernel
		add	r0, r0, #(TEXTADDR & 0x00f00000) >> 18
		str	r3, [r0], #4			@ PAGE_OFFSET + 0MB
		add	r3, r3, #1 << 20
		str	r3, [r0], #4			@ PAGE_OFFSET + 1MB
		add	r3, r3, #1 << 20
		str	r3, [r0], #4			@ PAGE_OFFSET + 2MB
		add	r3, r3, #1 << 20
		str	r3, [r0], #4			@ PAGE_OFFSET + 3MB

		/*
		 * Ensure that the first section of RAM is present.
		 * we assume that:
		 *  1. the RAM is aligned to a 256MB boundary
		 *  2. the kernel is executing in the same 256MB chunk
		 *     as the start of RAM.
		 */
		bic	r0, r0, #0x0ff00000 >> 18	@ round down
		and	r2, r5, #0xf0000000		@ round down
		add	r3, r8, r2			@ flags + rambase
		str	r3, [r0]

		bic	r8, r8, #0x0c			@ turn off cacheable
							@ and bufferable bits
#ifdef CONFIG_DEBUG_LL
		/*
		 * Map in IO space for serial debugging.
		 * This allows debug messages to be output
		 * via a serial console before paging_init.
		 */
		add	r0, r4, r7
		rsb	r3, r7, #0x4000	@ PTRS_PER_PGD*sizeof(long)
		cmp	r3, #0x0800
		addge	r2, r0, #0x0800
		addlt	r2, r0, r3
		orr	r3, r6, r8
1:		str	r3, [r0], #4
		add	r3, r3, #1 << 20
		teq	r0, r2
		bne	1b
#endif
		mov	pc, lr
#endif


/*
 * Exception handling.  Something went wrong and we can't
 * proceed.  We ought to tell the user, but since we
 * don't have any guarantee that we're even running on
 * the right architecture, we do virtually nothing.
 * r0 = ascii error character:
 *	a = invalid architecture
 *	p = invalid processor
 *	i = invalid calling convention
 *
 * Generally, only serious errors cause this.
 */
__error:
#ifdef CONFIG_DEBUG_LL
		mov	r8, r0				@ preserve r0
		adr	r0,err_str
		bl	printascii
		mov	r0, r8
		bl	printch
#endif
1:		mov	r0, r0
		b	1b

#ifdef CONFIG_DEBUG_LL
__greet:
	mov r8, r0
	mov r9, lr
	adr r0, greet_str
	bl printascii
	mov r0, r8
	mov pc, r9

 .align
err_str:	.asciz	"Error "
greet_str:	.asciz	"\nWelcome to Av3XX uclinux debugger - by Oxygen77\n"
#endif

.align
/*
 * Read processor ID register (CP#15, CR0), and look up in the linker-built
 * supported processor list.  Note that we can't use the absolute addresses
 * for the __proc_info lists since we aren't running with the MMU on
 * (and therefore, we are not in the correct address space).  We have to
 * calculate the offset.
 *
 * Returns:
 *	r5, r6, r7 corrupted
 *	r8  = page table flags
 *	r9  = processor ID
 *	r10 = pointer to processor structure
 */
__lookup_processor_type:
		adr	r5, 2f
		ldmia	r5, {r7, r9, r10}
		sub	r5, r5, r10			@ convert addresses
		add	r7, r7, r5			@ to our address space
		add	r10, r9, r5
#ifdef CONFIG_CPU_WITH_MCR_INSTRUCTION
		mrc	p15, 0, r9, c0, c0		@ get processor id
#else
		@ A possible code
		@ldr	r9, PROCESSOR_ID_MEM_LOCATION
		@ldr	r9, [r9]
#ifdef CONFIG_ARCH_AV3XX
		ldr r9, L_AV3XX_PID
#else
# warning 	"FIXME: Get Processor ID without MCR Instruction"
#endif

#endif
1:		ldmia	r10, {r5, r6, r8}		@ value, mask, mmuflags
		and	r6, r6, r9			@ mask wanted bits
		teq	r5, r6
		moveq	pc, lr
		add	r10, r10, #36			@ sizeof(proc_info_list)
		cmp	r10, r7
		blt	1b
		mov	r10, #0				@ unknown processor
		mov	pc, lr

/*
 * Look in include/asm-arm/procinfo.h and arch/arm/kernel/arch.[ch] for
 * more information about the __proc_info and __arch_info structures.
 */
2:		.long	__proc_info_end
		.long	__proc_info_begin
		.long	2b
		.long	__arch_info_begin
		.long	__arch_info_end

/*
 * Lookup machine architecture in the linker-build list of architectures.
 * Note that we can't use the absolute addresses for the __arch_info
 * lists since we aren't running with the MMU on (and therefore, we are
 * not in the correct address space).  We have to calculate the offset.
 *
 *  r1 = machine architecture number
 * Returns:
 *  r2, r3, r4 corrupted
 *  r5 = physical start address of RAM
 *  r6 = physical address of IO
 *  r7 = byte offset into page tables for IO
 */
__lookup_architecture_type:
#ifdef CONFIG_ARCH_AV3XX
		ldr r1, L_AV3XX_MID
#endif
		adr	r4, 2b
		ldmia	r4, {r2, r3, r5, r6, r7}	@ throw away r2, r3
		sub	r5, r4, r5			@ convert addresses
		add	r4, r6, r5			@ to our address space
		add	r7, r7, r5
1:		ldr	r5, [r4]			@ get machine type
		teq	r5, r1
		beq	2f
		add	r4, r4, #SIZEOF_MACHINE_DESC
		cmp	r4, r7
		blt	1b
		mov	r7, #0				@ unknown architecture
		mov	pc, lr
2:		ldmib	r4, {r5, r6, r7}		@ found, get results
		mov	r7, r7, lsr #18			@ pagetable byte offset
		mov	pc, lr

#ifdef CONFIG_ARCH_AV3XX
L_AV3XX_PID:	.long	0x41007700
L_AV3XX_MID:	.long	MACH_TYPE_AV3XX
#endif
@.align 4
.globl av3xx_startup_scr
av3xx_startup_scr:
.incbin "../mach-av3xx/boot_halt_img/startup_screen.inc"
@.align 4
@.globl av3xx_start_scr
@av3xx_start_scr:
@.incbin "../mach-av3xx/boot_halt_img/start_screen.inc"
@.align 4
.globl av3xx_halt_scr
av3xx_halt_scr:
.incbin "../mach-av3xx/boot_halt_img/halt_screen.inc"

#ifdef CONFIG_BLK_DEV_BLKMEM
.section ".rodata"
	.align	12
	.globl	__ramdisk_data
__ramdisk_data:
.incbin "../mach-av3xx/rom.inc"
	.globl	__ramdisk_data_end
__ramdisk_data_end:
	.align
#endif

