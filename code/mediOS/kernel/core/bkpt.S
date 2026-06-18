.text

.global do_bkpt
do_bkpt:
    bkpt 0
    mov PC, LR
    