.section .data
.align 4
myval:
    .word 42

.section .text
.global _start
_start:
    lla x8, myval         # x8 = &myval
    # c.lw x10, x8, 0      # x10 (a0) = *x8
    # .byte 0xff, 0xff, 0xff, 0xfe
    # 0x80048067
    # jalr x0, -2048(x9)
    .byte 0x47, 0x80, 0x04, 0x84
    li a7, 93            # syscall: exit
    ecall

