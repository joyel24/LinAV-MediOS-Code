
.text
#ifdef INCLUDE_IMG
.align 4
.globl bg_img
bg_img:
.incbin "img.inc"
#endif
