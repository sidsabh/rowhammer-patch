    .section .text
    .global _start

_start:
    # Load the address of 'value_storage' into a2
    addi sp, sp, -4    # Allocate space on stack
    li t0, 42          # Load 42 into t0
    sw t0, 0(sp)       # Store 42 at stack top
    mv a2, sp          # Load stack address into a2

    # secret 1
    addi sp, sp, -4   
    li t0, 101 # Super secret value
    sw t0, 0(sp)       
    mv a4, sp          

    lw a1, 0(a2)  
    
    # Syscall: exit with a1 as exit code
    li a7, 93        # Syscall number for exit
    mv a0, a1        # Move loaded value into exit code
    ecall            # Invoke syscall


