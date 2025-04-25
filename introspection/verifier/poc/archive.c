#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
// #include <gelf.h>
// #include <libelf.h>
#include <openssl/sha.h>

// unsigned char CT_HASH[SHA256_DIGEST_LENGTH] = {
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
// };

#define TOP_32_20 0xFFFFF
#define TOP_32_12 0xFFF
#define BOTTOM_32_20 0xFFFFF
#define BOTTOM_32_12 0xFFF


extern char __text_start;
extern char __text_end;

__attribute__((constructor))
__attribute__((section(".verify")))
void verify() {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // print pc
    // printf("PC: %p\n", __builtin_return_address(0));

    // print the address of __text_start and __text_end
    // printf("Address of __text_start: %p\n", &__text_start);
    // printf("Address of __text_end: %p\n", &__text_end);

    // Hash the memory range of the .text section
    SHA256((unsigned char *)&__text_start,
           (size_t)(&__text_end - &__text_start),
           hash);

    // Compare computed hash with CT_HASH
    
    // Compress hash into 64-bit rt_hash_val
    unsigned long long rt_hash_val = 0;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        rt_hash_val |= ((unsigned long long)hash[i] << (8 * (SHA256_DIGEST_LENGTH - 1 - i)));
    }

    // put an INLINE ASSEMBLY HERE in RISC-V
    __asm__ volatile (

        // --- Build PC + CT_HASH into x20 (jump_to) ---
        "auipc x20, BOTTOM_32_20\n"          // CT_HASH[31:12] (low part)
        "li    x23, BOTTOM_32_12\n"
        "or    x20, x20, x23\n"

        "lui   x23, TOP_32_20\n"          // upper 32 bits of CT_HASH
        "li    x25, TOP_32_12\n"
        "or    x23, x23, x25\n"
        "slli  x23, x23, 32\n"
        "add   x20, x20, x23\n"

        // --- Load runtime hash into x25 ---
        "mv x25, %[rh]\n"

        // --- Subtract runtime hash ---
        "sub   x20, x20, x25\n"

        // --- Add offset ---
        "addi  x20, x20, 0x2a\n"

        // --- Jump if verified ---
        "jalr  x0, x20, 0\n"

        // --- Fault sled ---
        ".word 0xFFFFFFFF\n"

        :
        : [rh] "r" (rt_hash_val)
        : "x20", "x23", "x25"
    );


    // if (memcmp(hash, CT_HASH, SHA256_DIGEST_LENGTH) != 0) {
    //     fprintf(stderr, "Hash mismatch! Exiting...\nComputed: ");
    //     for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    //         printf("%02x", hash[i]);
    //     }
    //     printf("\n");
    //     exit(EXIT_FAILURE);
    // } else {
    //     printf("Hash verified.\n");
    // }
}



// __attribute__((section(".verify")))
// void verify() {
//     int fd = open("/proc/self/exe", O_RDONLY);
//     if (fd < 0) {
//         perror("open");
//         exit(EXIT_FAILURE);
//     }

//     if (elf_version(EV_CURRENT) == EV_NONE) {
//         fprintf(stderr, "ELF library initialization failed\n");
//         exit(EXIT_FAILURE);
//     }

//     Elf *e = elf_begin(fd, ELF_C_READ, NULL);
//     if (!e) {
//         fprintf(stderr, "elf_begin failed: %s\n", elf_errmsg(-1));
//         exit(EXIT_FAILURE);
//     }

//     size_t shstrndx;
//     if (elf_getshdrstrndx(e, &shstrndx) != 0) {
//         fprintf(stderr, "elf_getshdrstrndx failed\n");
//         exit(EXIT_FAILURE);
//     }
//     SHA256((unsigned char *)&__text_start,
//        (size_t)(&__text_end - &__text_start),
//        hash);

//     Elf_Scn *scn = NULL;
//     GElf_Shdr shdr;
//     char *name;
//     unsigned char hash[SHA256_DIGEST_LENGTH];

//     while ((scn = elf_nextscn(e, scn)) != NULL) {
//         gelf_getshdr(scn, &shdr);
//         name = elf_strptr(e, shstrndx, shdr.sh_name);
//         if (strcmp(name, ".text") == 0) {
//             lseek(fd, shdr.sh_offset, SEEK_SET);
//             unsigned char *buf = malloc(shdr.sh_size);
//             read(fd, buf, shdr.sh_size);

//             SHA256(buf, shdr.sh_size, hash);
//             free(buf);

//             if (memcmp(hash, CT_HASH, SHA256_DIGEST_LENGTH) != 0) {
//                 fprintf(stderr, "Hash mismatch! Exiting...\n");
//                 for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//                     printf("%02x", hash[i]);
//                 }
//                 printf("\n");
//                 exit(EXIT_FAILURE);
//             } else {
//                 printf("Hash verified.\n");
//             }

//             break;
//         }
//     }

//     elf_end(e);
//     close(fd);
// }


// __attribute__((section(".verify")))
// __attribute__((constructor))
// void run_verify() {
//     verify();
// }
