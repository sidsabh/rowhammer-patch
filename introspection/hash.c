#include <stdint.h>
#include <stddef.h>

uint32_t main(const uint32_t *data, size_t len) {
    const uint32_t p = 131;        // small prime base
    const uint32_t mod = 1UL << 30; // 2^30
    uint64_t hash = 0;
    uint64_t power = 1;

    for (size_t i = 0; i < len; ++i) {
        hash = (hash + (uint64_t)data[i] * power) % mod;
        power = (power * p) % mod;
    }

    return (uint32_t)hash;
}

