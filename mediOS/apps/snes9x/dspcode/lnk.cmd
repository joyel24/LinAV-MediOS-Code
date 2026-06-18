-stack 0x0800 /* size of .stack section */
-heap  0x3800 /* size of .sysmem section */

MEMORY {
   PAGE 0: /* program memory */

	DARAM (RWX) : origin = 0x0100, length = 0x7E80
/*	DARAM (RWX) : origin = 0x1100, length = 0x6E80 */

	/* boot interrupt vector table location */
	VECTORS (RWX): origin = 0x7F80, length = 0x80

   PAGE 0: /* source memory */

/*	SOURCE (RW) : origin = 0x8000, length = 0x0200*/

   PAGE 0: /* image buffer memory */

/*	IMGBUF (RW) : origin = 0x8200, length = 0x0600*/

   PAGE 0: /* heap memory */

	FFT  (RW) : origin = 0x8000, length = 0x0400
	HEAP (RW) : origin = 0x8400, length = 0x3C00
/*	HEAP (RW) : origin = 0x8800, length = 0x3800 */
	CODE (X) : origin = 0xc000, length = 0x3800 

   PAGE 1:

/*  DATA_RAM (RW): origin = 0x4000, length = 0x2C00
  DATA_EXT (RW): origin = 0x8000, length = 0x7FFF */

} /* MEMORY */

SECTIONS {
   .fft     > FFT     PAGE 0 /* fft buffer                */
   .text    > DARAM   PAGE 0 /* code                      */
   .switch  > DARAM   PAGE 0 /* switch table info         */
   .vectors > VECTORS PAGE 0 /* interrupt vectors         */
   .cio     > DARAM   PAGE 0 /* C I/O                     */  
   .data    > HEAP    PAGE 0 /* initialized data          */
   .sintab  > DARAM   PAGE 0 /* dsplib fft data           */
   .bss     > HEAP    PAGE 0 /* global & static variables */
   .const   > HEAP    PAGE 0 /* constant data             */
   .sysmem  > HEAP    PAGE 0 /* heap                      */
   .stack   > HEAP    PAGE 0 /* stack                     */
   .cinit   > DARAM   PAGE 0 /* cinit                     */
   .csldata > HEAP    PAGE 0
} /* SECTIONS */
