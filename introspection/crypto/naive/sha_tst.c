#include "riscv_crypto.h"
#include <stdint.h>
#include <stdio.h>

int main() {
    uint32_t x = 0x6a09e667;
    printf("sha256sig0 = %08x\n", __riscv_sha256sig0(x));
    printf("sha256sig1 = %08x\n", __riscv_sha256sig1(x));
    printf("sha256sum0 = %08x\n", __riscv_sha256sum0(x));
    printf("sha256sum1 = %08x\n", __riscv_sha256sum1(x));
    return 0;
}

