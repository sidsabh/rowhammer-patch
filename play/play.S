    .section .text
    .global _start

_start:
    # Syscall: write(STDOUT, helloworld, 13)
    li a7, 64        # syscall number for write
    li a0, 1         # file descriptor (stdout)
    lla a1, helloworld  # load address of the string
    li a2, 13        # length of the string
    ecall            # invoke syscall

    # Syscall: exit(0)
    li a7, 93        # syscall number for exit
    li a0, 0         # exit status
    ecall            # invoke syscall

.section .data
helloworld:
    .ascii "Hello World\n"

