#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MODULO (1UL << 30)
#define BASE 131

uint32_t poly_rolling_hash(const uint8_t *bytes, size_t len) {
    uint64_t hash = 0;
    uint64_t power = 1;

    // Process every 4 bytes as a 32-bit word (little-endian)
    for (size_t i = 0; i < len; i += 4) {
        uint32_t word = 0;
        for (int j = 0; j < 4 && i + j < len; ++j) {
            word |= ((uint32_t)bytes[i + j]) << (8 * j);
        }
        hash = (hash + word * power) % MODULO;
        power = (power * BASE) % MODULO;
    }

    return (uint32_t)hash;
}

int main() {
    char buffer[4096];

    printf("Enter input: ");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        fprintf(stderr, "Failed to read input\n");
        return 1;
    }

    size_t len = strcspn(buffer, "\n"); // Get input up to newline
    buffer[len] = '\0'; // Null-terminate at newline just in case

    uint32_t hash = poly_rolling_hash((uint8_t *)buffer, len);
    printf("Hash: %u\n", hash);
    return 0;
}

