-stack 0x0800 /* size of .stack section */
-heap  0x3800 /* size of .sysmem section */

-o aosms_dsp.out

MEMORY {
   PAGE 0: /* program memory */

	DARAM (RWX) : origin = 0x0100, length = 0x7E80

	/* boot interrupt vector table location */
	VECTORS (RWX): origin = 0x7F80, length = 0x80

   PAGE 0: /* source memory */

	SOURCE (RW) : origin = 0x8000, length = 0x0200

   PAGE 0: /* image buffer memory */

	IMGBUF (RW) : origin = 0x8200, length = 0x0600

   PAGE 0: /* heap memory */

	HEAP (RW) : origin = 0x8800, length = 0x3800

   PAGE 1:

  DATA_RAM (RW): origin = 0x4000, length = 0x2C00
  DATA_EXT (RW): origin = 0x8000, length = 0x7FFF

} /* MEMORY */

SECTIONS {
   .text    > DARAM   PAGE 0 /* code                     */
   .switch  > DARAM   PAGE 0 /* switch table info        */
   .vectors > VECTORS PAGE 0 /* interrupt vectors         */
   .cio     > DARAM   PAGE 0 /* C I/O                     */  
   .data    > DARAM   PAGE 0 /* initialized data          */
   .bss     > DARAM   PAGE 0 /* global & static variables */
   .const   > DARAM   PAGE 0 /* constant data             */
   .sysmem  > HEAP    PAGE 0 /* heap                      */
   .stack   > DARAM   PAGE 0 /* stack                     */
   .cinit   > DARAM   PAGE 0 /* cinit                     */
	 .csldata > DARAM 	PAGE 0
} /* SECTIONS */
