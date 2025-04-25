#define DEBUG
// #define LD_VERSION 1

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

#ifdef DEBUG
#define DPRINT(...) printf(__VA_ARGS__)
#else
#define DPRINT(...)
#endif

#ifdef DEBUG
#include <stdio.h>
#endif

#include <openssl/sha.h>

#ifdef LD_VERSION
extern char __text_start, __text_end;
#else 
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <gelf.h>
#include <libelf.h>
#include <unistd.h>
void get_hash_via_elf(unsigned char *hash);
#endif

__attribute__((constructor))
__attribute__((section(".verify")))
void verify() {

    DPRINT("TOP_32_20: %x\n", TOP_32_20);
    DPRINT("TOP_32_12: %x\n", TOP_32_12);
    DPRINT("BOTTOM_32_20: %x\n", BOTTOM_32_20);
    DPRINT("BOTTOM_32_12: %x\n", BOTTOM_32_12);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    // get the hash of the text section
#ifdef LD_VERSION
    SHA256((unsigned char*)&__text_start,
           (size_t)(&__text_end - &__text_start),
           hash);
#else
    get_hash_via_elf(hash);
#endif


    // print hash
    DPRINT("Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        DPRINT("%02x", hash[i]);
    }
    DPRINT("\n");

    uint64_t rt_hash_val = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t chunk = 0;
        for (int j = 0; j < 8; j++) {
            chunk = (chunk << 8) | hash[i * 8 + j];
        }
        rt_hash_val ^= chunk;
    }
    //print this value
    DPRINT("rt_hash_val: %llx\n", rt_hash_val);
    
#ifdef DEBUG
    uint64_t pc = 0;
    __asm__ volatile (
        "auipc %0, 0\n"
        : "=r"(pc)
    );
    // print the value of pc
    printf("PC: %llx\n", pc);
#endif

    DPRINT("BOTTOM_32_19: %x\n", BOTTOM_32_19);
    DPRINT("BOTTOM_32_1: %x\n", BOTTOM_32_1);
    DPRINT("BOTTOM_32L_11: %x\n", BOTTOM_32L_11);
    DPRINT("BOTTOM_32L_0: %x\n", BOTTOM_32L_0);
    DPRINT("TOP_32_19: %x\n", TOP_32_19);
    DPRINT("TOP_32_1: %x\n", TOP_32_1);
    DPRINT("TOP_32L_11: %x\n", TOP_32L_11);
    DPRINT("TOP_32L_0: %x\n", TOP_32L_0);

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
        "srai  x23, x23, 11\n"
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
        "srai  x23, x23, 11\n"
        "ori   x23, x23, " STR(TOP_32_1) "\n"
        "slli  x23, x23, 44\n"
        "add   x20, x20, x23\n"
    
        /* rt_hash_val → x25 */
        "mv    x25, %0\n"
        /* (PC+CT) – RT */
        "sub   x20, x20, x25\n"
        /* skip offset */
        "addi  x20, x20, 0x50\n"
        "jalr  x0, x20, 0\n"
        /* faulter */
        ".word 0xFFFFFFFF\n"
        : /* no outputs */
        : "r"(rt_hash_val)
        : "x20","x23","x25"
    );
    
}


#ifndef LD_VERSION
void get_hash_via_elf(unsigned char *hash) {
    int fd = open("/proc/self/exe", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (elf_version(EV_CURRENT) == EV_NONE) {
        fprintf(stderr, "ELF library initialization failed\n");
        exit(EXIT_FAILURE);
    }

    Elf *e = elf_begin(fd, ELF_C_READ, NULL);
    if (!e) {
        fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
        exit(EXIT_FAILURE);
    }

    size_t shstrndx;
    if (elf_getshdrstrndx(e, &shstrndx) != 0) {
        fprintf(stderr, "elf_getshdrstrndx failed\n");
        exit(EXIT_FAILURE);
    }

    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;
    char *name;

    while ((scn = elf_nextscn(e, scn)) != NULL) {
        gelf_getshdr(scn, &shdr);
        name = elf_strptr(e, shstrndx, shdr.sh_name);
        if (strcmp(name, ".text") == 0) {
            lseek(fd, shdr.sh_offset, SEEK_SET);
            unsigned char *buf = malloc(shdr.sh_size);
            read(fd, buf, shdr.sh_size);

            SHA256(buf, shdr.sh_size, hash);
            free(buf);
    }}
}
#endif