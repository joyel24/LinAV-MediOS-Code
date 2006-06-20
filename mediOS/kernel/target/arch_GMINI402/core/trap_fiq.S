.section ".core"

@.align	5
.global trap_fiq

trap_fiq: @called every 500ns

  ldr     r9, =0x30500

  ldrh    r8, [r9]        @DM270_INTC_FIQ0
  tst     r8, #8          @fiq 3 set?
  bne     trap_fiq_end
  ldrh    r8, [r9,#0x28]  @DM270_INTC_IRQ0
  tst     r8, #8          @irq 3 enabled?
  beq     trap_fiq_end

  ldrh    r8, [r9]
  orr     r8, r8, #0x08
  strh    r8, [r9]        @set fiq 3 (timer 3)

  ldrh    r10, [r9,#0x8c] @DM270_GIO_OFFSET_BITSET0
  ldr     r8, =.offCounter
  tst     r10, #8         @OFF pressed?
  bne     off_not_pressed
  ldr     r10, [r8]
  add     r10, r10, #1
  str     r10, [r8]       @inc off counter
  ldr     r10,=20000      @10 seconds
  ldr     r8, [r8]
  cmp     r8, r10         @off was pressed enough time?
  bls     no_shutdown
  ldrh    r8, [r9,#0x28]
  and     r10, r8, #0xf7
  and     r8, r8, #0xff00
  orr     r8, r10, r8
  strh    r8, [r9,#0x28]  @disable irq3

wait_shutdown:
  b       wait_shutdown

off_not_pressed:
  mov     r10, #0
  str     r10, [r8]

no_shutdown:
  ldr     r8, =.wdtCounter
  ldr     r10, [r8]
  tst     r10, #2         @every 2 ticks (1ms), change gio bit 5 state (watchdog bit)
  moveq   r10, #0x20
  streqh  r10, [r9,#0x92] @DM270_GIO_OFFSET_BITCLR0
  movne   r10, #0x20
  strneh  r10, [r9,#0x8c] @DM270_GIO_OFFSET_BITSET0
  ldr     r10, [r8]
  add     r10, r10, #1
  str     r10, [r8]       @inc wdt counter

trap_fiq_end:
  subs    pc, lr, #4

.offCounter:	.long	0x0
.wdtCounter:	.long	0x0
