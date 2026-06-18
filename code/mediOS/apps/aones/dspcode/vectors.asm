;**********************************************************************
;*		 (C) COPYRIGHT TEXAS INSTRUMENTS, INC. 1996 				  *
;*																	  *
;*		Interrupt Vector Assignments for 'C54x						  *
;* 
;* Note that not all interupt names will apply to any given target.
;* This serves only as a multi-purpose example across the C54x family.
;* Consult the TI DSP Reference manual to determine which interrupts 
;*   apply to a given target.
;**********************************************************************
	.title "Interrupt Vectors w/ RTDX for C54X"
	.tab	4
	.file	"vectors.asm"
	.mmregs

	.sect	".vectors"
	.def RS_V, SINTR_V
	.ref _c_int00
RS_V:				; Hardware Reset Vector
SINTR_V:			; Software Reset Vector
	; Note: no need to push XPC here - reset clears XPC
	.if ((__far_mode) & ($isdefed("_BIOSHWINEAR") = 0))
	FBD		_c_int00			; Branch to MainLine.
	.else
	BD		_c_int00			; Branch to MainLine.
	.endif
	STM		#0100h, SP			; initialize Stack Pointer

	.def NMI_V, SINT16_V
NMI_V:				; Non-maskable interrupt Vector
SINT16_V:			; Software Interrupt #16 Vector
	B		$
	NOP
	NOP

	.end
