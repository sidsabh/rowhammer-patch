#ifndef TOP_32_20
#define TOP_32_20 0xFFFFF
#endif
#ifndef TOP_32_12
#define TOP_32_12 0xFFF
#endif
#ifndef BOTTOM_32_20
#define BOTTOM_32_20 0xFFFFF
#endif
#ifndef BOTTOM_32_12
#define BOTTOM_32_12 0xFFF
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define DEBUG


#ifdef DEBUG
#include <stdio.h>
#endif
#include <openssl/sha.h>
extern char __text_start, __text_end;

__attribute__((constructor))
__attribute__((section(".verify")))
void verify() {

    // print the TOP_32_20 and TOP_32_12 and BOTTOM_32_20 and BOTTOM_32_12
#ifdef DEBUG
    printf("TOP_32_20: %x\n", TOP_32_20);
    printf("TOP_32_12: %x\n", TOP_32_12);
    printf("BOTTOM_32_20: %x\n", BOTTOM_32_20);
    printf("BOTTOM_32_12: %x\n", BOTTOM_32_12);
#endif

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)&__text_start,
           (size_t)(&__text_end - &__text_start),
           hash);


    // print hash
    printf("Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    uint64_t rt_hash_val = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t chunk = 0;
        for (int j = 0; j < 8; j++) {
            chunk = (chunk << 8) | hash[i * 8 + j];
        }
        rt_hash_val ^= chunk;
    }
    //print this value
    printf("rt_hash_val: %llx\n", rt_hash_val);
           
        __asm__ volatile (
            // --- Build PC + CT_HASH into x20 ---
            "auipc x20, " STR(BOTTOM_32_20) "\n"
            "li    x23, " STR(BOTTOM_32_12) "\n"
            "or    x20, x20, x23\n"
        
            "lui   x23, " STR(TOP_32_20) "\n"
            "li    x25, " STR(TOP_32_12) "\n"
            "or    x23, x23, x25\n"
            "slli  x23, x23, 32\n"
            "add   x20, x20, x23\n"
        
            "mv    x25, %0\n"
            "sub   x20, x20, x25\n"
            "addi  x20, x20, 0x2a\n"
            "jalr  x0, x20, 0\n"
            ".word 0xFFFFFFFF\n"
            :
            : "r"(rt_hash_val)
            : "x20", "x23", "x25"
        );
        
}
