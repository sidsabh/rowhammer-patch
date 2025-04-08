#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int main() {
    const char *data = "hello world";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Compute SHA-256 hash
    SHA256((const unsigned char *)data, strlen(data), hash);

    // Print hash in hex
    printf("SHA-256(\"%s\") = ", data);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", hash[i]);
    printf("\n");

    return 0;
}

