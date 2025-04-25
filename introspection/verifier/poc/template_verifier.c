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

#ifndef BOTTOM_32_19
#define BOTTOM_32_19 (BOTTOM_32_20 >> 1)
#endif
#ifndef BOTTOM_32_1
#define BOTTOM_32_1  (BOTTOM_32_20 & 1)
#endif
#ifndef BOTTOM_32L_11
#define BOTTOM_32L_11 (BOTTOM_32_12 >> 1)
#endif
#ifndef BOTTOM_32L_0
#define BOTTOM_32L_0  (BOTTOM_32_12 & 1)
#endif

#ifndef TOP_32_19
#define TOP_32_19    (TOP_32_20 >> 1)
#endif
#ifndef TOP_32_1
#define TOP_32_1     (TOP_32_20 & 1)
#endif
#ifndef TOP_32L_11
#define TOP_32L_11   (TOP_32_12 >> 1)
#endif
#ifndef TOP_32L_0
#define TOP_32L_0    (TOP_32_12 & 1)
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
    

    // print PC
    uint64_t pc = 0;
    __asm__ volatile (
        "auipc %0, 0\n"
        : "=r"(pc)
    );
    // print the value of pc
    printf("PC: %llx\n", pc);
           
    // uint64_t computed_target;
    // __asm__ volatile (
    //     // --- Build PC + CT_HASH into x20 ---
    //     "auipc x20, " STR(BOTTOM_32_20) "\n"
    //     "li    x23, " STR(BOTTOM_32_12) "\n"
    //     "or    x20, x20, x23\n"
    
    //     "lui   x23, " STR(TOP_32_20) "\n"
    //     "li    x25, " STR(TOP_32_12) "\n"
    //     "or    x23, x23, x25\n"
    //     "slli  x23, x23, 32\n"
    //     "add   x20, x20, x23\n"
    
    //     "mv    x25, %1\n"
    //     "sub   x20, x20, x25\n"
    //     "addi  x20, x20, 0x132\n"
    //     "mv %0, x20\n"
    //     : "=r"(computed_target)   // first outputs
    //     : "r"(rt_hash_val)        // then inputs
    //     : "x20", "x23", "x25"
    // );
//     #ifdef DEBUG
//     printf("computed jump_to %x\n", computed_target);
// #endif
    // print these guys
#ifdef DEBUG
    printf("BOTTOM_32_19: %x\n", BOTTOM_32_19);
    printf("BOTTOM_32_1: %x\n", BOTTOM_32_1);
    printf("BOTTOM_32L_11: %x\n", BOTTOM_32L_11);
    printf("BOTTOM_32L_0: %x\n", BOTTOM_32L_0);
    printf("TOP_32_19: %x\n", TOP_32_19);
    printf("TOP_32_1: %x\n", TOP_32_1);
    printf("TOP_32L_11: %x\n", TOP_32L_11);
    printf("TOP_32L_0: %x\n", TOP_32L_0);
#endif

    __asm__ volatile (
        /* x20 has PC */
        "auipc x20, 0x0\n"

        /* x23 has BOTTOM_32_12 */
        "li    x23, " STR(BOTTOM_32L_11) "\n"
        "slli  x23, x23, 1\n"
        "ori   x23, x23, " STR(BOTTOM_32L_0) "\n"
        "add  x20, x20, x23\n"

        /* x23 has BOTTOM_32_20 */
        "lui   x23, " STR(BOTTOM_32_19) "\n"
        "slli  x23, x23, 1\n"
        "ori   x23, x23, " STR(BOTTOM_32_1) "\n"
        "slli  x23, x23, 12\n"
        "add  x20, x20, x23\n"

        /* x23 has TOP_32_12 */
        "li    x23, " STR(TOP_32L_11) "\n"
        "slli  x23, x23, 1\n"
        "ori   x23, x23, " STR(TOP_32L_0) "\n"
        "slli  x23, x23, 32\n"
        "add   x20, x20, x23\n"

        /* x23 has TOP_32_20 */
        "lui   x23, " STR(TOP_32_19) "\n"
        "slli  x23, x23, 1\n"
        "ori   x23, x23, " STR(TOP_32_1) "\n"
        "slli  x23, x23, 44\n"
        "add   x20, x20, x23\n"

    
        /* rt_hash_val → x25 */
        "mv    x25, %0\n"
        /* (PC+CT) – RT */
        "sub   x20, x20, x25\n"
        /* skip offset */
        "addi  x20, x20, 0x42\n"
        "jalr  x0, x20, 0\n"
        /* faulter */
        ".word 0xFFFFFFFF\n"
        : /* no outputs */
        : "r"(rt_hash_val)
        : "x20","x23","x25"
    );
}


// __asm__ volatile (
//     // --- Build PC + CT_HASH into x20 ---
//     "auipc x20, " STR(BOTTOM_32_20) "\n"
//     "li    x23, " STR(BOTTOM_32_12) "\n"
//     "or    x20, x20, x23\n"

//     "lui   x23, " STR(TOP_32_20) "\n"
//     "li    x25, " STR(TOP_32_12) "\n"
//     "or    x23, x23, x25\n"
//     "slli  x23, x23, 32\n"
//     "add   x20, x20, x23\n"

//     "mv    x25, %1\n"
//     "sub   x20, x20, x25\n"
//     "addi  x20, x20, 0x2a\n"
//     "mv %0, x20\n"
//     : "=r"(computed_target)   // first outputs
//     : "r"(rt_hash_val)        // then inputs
//     : "x20", "x23", "x25"
// );