.section .text
.global _start
.type _start, @function
_start:
    # call main()
    call main

    # exit(main())
    mv a0, a0         # return value from main is already in a0
    li a7, 93         # syscall number for exit
    ecall

