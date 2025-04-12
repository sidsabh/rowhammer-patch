#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <gelf.h>
#include <libelf.h>
#include <openssl/sha.h>

unsigned char CT_HASH[SHA256_DIGEST_LENGTH] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};


extern char __text_start;
extern char __text_end;

__attribute__((constructor))
__attribute__((section(".verify")))
void verify() {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // print pc
    printf("PC: %p\n", __builtin_return_address(0));

    // print the address of __text_start and __text_end
    printf("Address of __text_start: %p\n", &__text_start);
    printf("Address of __text_end: %p\n", &__text_end);

    // Hash the memory range of the .text section
    SHA256((unsigned char *)&__text_start,
           (size_t)(&__text_end - &__text_start),
           hash);

    // Compare computed hash with CT_HASH
    if (memcmp(hash, CT_HASH, SHA256_DIGEST_LENGTH) != 0) {
        fprintf(stderr, "Hash mismatch! Exiting...\nComputed: ");
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            printf("%02x", hash[i]);
        }
        printf("\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Hash verified.\n");
    }
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
