#!/bin/bash
set -e

# Set SYSROOT to the first argument, or default
SYSROOT="${1:-/home/prachi/sysroot-link}"

# Print the initial build message
echo "[1] Initial build (no hash)..."

make -s clean

# Set a default value for INFILE if it's not already set
: "${INFILE:=tests/simple.c}"

# Run the make command with the provided arguments
make -s INFILE="$INFILE" OUTFILE=verified_main.elf SYSROOT="$SYSROOT"

echo "[2] Hashing .text section and deriving constants..."

read -r CT_HASH RT_HEX TOP_20 TOP_12 BOT_20 BOT_12 <<<$(python3 - <<EOF
import lief, hashlib

elf = lief.parse("verified_main.elf")
text = elf.get_section(".text").content
h = hashlib.sha256(bytes(text)).digest()
print(h.hex(), end=" ")

# XOR 4 64-bit chunks to compress
chunks = [int.from_bytes(h[i:i+8], byteorder='big') for i in range(0, 32, 8)]
rt_hash = chunks[0] ^ chunks[1] ^ chunks[2] ^ chunks[3]

top_32 = (rt_hash >> 32) & 0xFFFFFFFF
bot_32 = rt_hash & 0xFFFFFFFF

top_20 = (top_32 >> 12) & 0xFFFFF
top_12 = top_32 & 0xFFF
bot_20 = (bot_32 >> 12) & 0xFFFFF
bot_12 = bot_32 & 0xFFF

print(hex(rt_hash), hex(top_20), hex(top_12), hex(bot_20), hex(bot_12))
EOF
)

echo "[3] CT_HASH: $CT_HASH"
echo "[4] Derived rt_hash_val: $RT_HEX"
echo "[5] Recompiling with macros:"
echo "TOP_32_20=$TOP_20"
echo "TOP_32_12=$TOP_12"
echo "BOTTOM_32_20=$BOT_20"
echo "BOTTOM_32_12=$BOT_12"

touch template_verifier.c
make -s FINAL_BUILD=1 \
     INFILE="$INFILE" \
     OUTFILE=patched_main.elf \
     CT_MACROS="-DTOP_32_20=${TOP_20} -DTOP_32_12=${TOP_12} \
                -DBOTTOM_32_20=${BOT_20} -DBOTTOM_32_12=${BOT_12}"

echo "[✓] Final binary rebuilt: patched_main.elf"
