-stack 0x0800 /* size of .stack section */
-heap  0x1000 /* size of .sysmem section */

-o doom_dsp.out

MEMORY {
   PAGE 0: /* program memory */

	DARAM (RWX) : origin = 0x0100, length = 0x7E80

	/* boot interrupt vector table location */
	VECTORS (RWX): origin = 0x7F80, length = 0x80

   PAGE 0: /* heap memory */

	SARAM_D (RW) : origin = 0x8000, length = 0x3FFF

   PAGE 1:

	SARAM_C (RWX): origin = 0xC000, length = 0x3FFF

} /* MEMORY */

SECTIONS {
   .text    > DARAM   PAGE 0 /* code                     */
   .switch  > DARAM   PAGE 0 /* switch table info        */
   .cio     > DARAM   PAGE 0 /* C I/O                    */  
   .vectors > VECTORS PAGE 0 /* interrupt vectors        */
   .data    > DARAM   PAGE 0 /* initialized data         */
   .bss     > DARAM   PAGE 0 /* global & static variables*/
   .const   > DARAM   PAGE 0 /* constant data            */
   .sysmem  > DARAM   PAGE 0 /* heap                     */
   .stack   > DARAM   PAGE 0 /* stack                    */
   .cinit   > DARAM   PAGE 0 /* cinit                    */
   .csldata > DARAM   PAGE 0
   .saram_d > SARAM_D PAGE 0 /* data saram               */
} /* SECTIONS */
