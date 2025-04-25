#!/bin/bash

set -e

export C_INCLUDE_PATH=/usr/riscv64-linux-gnu/include
export LIBRARY_PATH=/usr/riscv64-linux-gnu/lib
export LD_LIBRARY_PATH=/home/sidsabh/code/security/rowhammer-project/riscv/sysroot/usr/lib

./build.sh

echo "[5] Flipping bit in patched_main.elf..."

python3 - <<EOF
import sys
import lief

def flip_bit(byte, bit_position):
    return byte ^ (1 << bit_position)

def vma_to_file_offset(elf, vma):
    for segment in elf.segments:
        seg_start = segment.virtual_address
        seg_end = seg_start + segment.virtual_size
        if seg_start <= vma < seg_end:
            return segment.file_offset + (vma - seg_start)
    raise ValueError(f"VMA {hex(vma)} not found in any segment")

def flip_bit_in_text_section(input_path, output_path, vma, bit_position):
    elf = lief.parse(input_path)
    if elf is None:
        print(f"❌ Failed to parse ELF: {input_path}")
        sys.exit(1)

    file_offset = vma_to_file_offset(elf, vma)

    with open(input_path, "rb") as f:
        data = bytearray(f.read())

    if file_offset >= len(data):
        print(f"❌ File offset {hex(file_offset)} is out of range")
        sys.exit(1)

    original_byte = data[file_offset]
    data[file_offset] = flip_bit(original_byte, bit_position)

    with open(output_path, "wb") as f:
        f.write(data)

    print(f"✅ Flipped bit {bit_position} at VMA {hex(vma)} (file offset {hex(file_offset)})")
    print(f"📂 Saved to {output_path}")

flip_bit_in_text_section("./patched_main.elf", "./altered_main.elf", 0x400100, 0)
EOF

echo "[✓] Bit flip complete: altered_main.elf"

chmod +x altered_main.elf
chmod +x patched_main.elf
echo "[✓] Executing altered_main.elf and patched_main.elf with QEMU..."
qemu-riscv64 -L /usr/riscv64-linux-gnu ./patched_main.elf
qemu-riscv64 -L /usr/riscv64-linux-gnu ./altered_main.elf
