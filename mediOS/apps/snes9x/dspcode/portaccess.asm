	.title "access to io space"
	.tab	4
	.file	"portaccess.asm"
	.mmregs
	.if __far_mode
offset	.set 1
	.else
offset	.set 0
	.endif

	.sect	".text"
	.global _portW, _portR, _dma_dsp2sdram, _dma_pending, _test_and_set

	
	.asg	(3+ offset), arg_port
	.asg	(3+ offset), arg_low
	.asg	(4+ offset), arg_hi
	.asg	(5+ offset), arg_length
	.asg	(6+ offset), arg_direction

	.asg	(3+ offset), arg_1
	.asg	(4+ offset), arg_2
	.asg	(5+ offset), arg_3
	.asg	(6+ offset), arg_4
	
	.asg	ar2,ar_value
	.asg	ar1,ar_1
	.asg	ar2,ar_2

;;; int test_and_set(int *, int value)
;;; doesn't work
_test_and_set:
        PSHM    ST0
        PSHM    ST1
        RSBX    OVA
        RSBX    OVB

	rsbx	sxm


	stlm	a, ar2
	ld	*sp(arg_1), a
	
	st	a,*ar2
     || ld	*ar2,a
	
	
        POPM    ST1
        POPM    ST0
	.if	__far_mode
	fretd
	.else
	retd
	.endif
	nop
	nop
		

;; /* direction : 1 dsp-->sdram, 0 sdram-->dsp */
;; void dma_dsp2sdram(void * dsp_addr, long sdram_addr, short length, short direction);
_dma_dsp2sdram:
	.if 1
        PSHM    ST0
        PSHM    ST1
        RSBX    OVA
        RSBX    OVB

	rsbx	sxm
	.endif


	portw	*sp(arg_hi), #080h	; sdram offset (hi)
	portw	*sp(arg_low), #081h	; sdram offset (low)

	stlm	a, ar2
	portw	*(ar2), #082h		; dsp ram offset
	stm	#0, ar2
	portw	*(ar2), #083h		; dsp ram page (?)
	portw	*sp(arg_length), #084h	; dma transfert length
	portw	*sp(arg_direction), #085h ; dma transfert direction

	stm	#1, ar2
	portw	*(ar2), #086h		; trigger dma

; 	portr	#086h, *(ar3)		; wait
;	portr	#086h, *(ar3)		; wait

	.if 1
        POPM    ST1
        POPM    ST0
	.endif
	.if	__far_mode
	fretd
	.else
	retd
	.endif
	nop
	nop

_dma_pending:
	.if 1
        PSHM    ST0
        PSHM    ST1
        RSBX    OVA
        RSBX    OVB

	rsbx	sxm
	.endif
	
	portr	#086h, *(ar_value)
	ldm	ar_value, a


	.if 1
        POPM    ST1
        POPM    ST0
	.endif
	.if	__far_mode
	fretd
	.else
	retd
	.endif
	nop
	nop



_portW:
        PSHM    ST0
        PSHM    ST1
        RSBX    OVA
        RSBX    OVB

	rsbx	sxm



	stlm	a, ar_value
	ld	*sp(arg_port), a
	stlm	a, portwinst+1	; code modification (find optimal amount of nop)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

portwinst:
	portw	*(ar_value), #080h
	
	

        POPM    ST1
        POPM    ST0
	.if	__far_mode
	fretd
	.else
	retd
	.endif
	nop
	nop

_portR:
        PSHM    ST0
        PSHM    ST1
        RSBX    OVA
        RSBX    OVB

	rsbx	sxm



	ld	*sp(arg_port), a
	stlm	a, portrinst+1	; code modification

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

portrinst:
	portr	#080h, *(ar_value)
	
	ldm	ar_value, a
	

        POPM    ST1
        POPM    ST0
	.if	__far_mode
	fretd
	.else
	retd
	.endif
	nop
	nop

	.end
