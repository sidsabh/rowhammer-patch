    .section .text
    .global _start

_start:
    # Load the address of 'value_storage' into a2
    addi sp, sp, -4    # Allocate space on stack
    li t0, 42          # Load 42 into t0
    sw t0, 0(sp)       # Store 42 at stack top

    mv a2, sp          # Load stack address into a2

    # Stack alignment (allocate 24 bytes instead of 20)
    addi sp, sp, -24
    sw x13, 0(sp)   # Save a4 (x13)
    sw x14, 4(sp)   # Save a5 (x14)
    sw x8,  8(sp)   # Save s0 (x8)
    sw x4,  12(sp)  # Save tp (x4)
    sw x28, 16(sp)  # Save t3 (x28)

    # Zero out registers before loading
    li x13, 0
    li x14, 0
    li x8, 0
    li x4, 0
    li x28, 0

    # Perform safe load (now loading from a valid memory address)
    lw a1, 0(a2)  

    # Restore registers
    lw x13, 0(sp)
    lw x14, 4(sp)
    lw x8,  8(sp)
    lw x4,  12(sp)
    lw x28, 16(sp)
    addi sp, sp, 24  # Stack realignment

    # Syscall: exit with a1 as exit code
    li a7, 93        # Syscall number for exit
    mv a0, a1        # Move loaded value into exit code
    ecall            # Invoke syscall

