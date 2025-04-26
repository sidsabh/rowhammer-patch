#!/bin/bash

# TO RUN: ./run.sh tests/<test_file>.c <optional sysroot dir> <optional num_rounds> <optional output_file>
INFILE="$1"
SYSROOT="${2:-/home/prachi/sysroot-link}"  # Default SYSROOT if not provided
NUM_ROUNDS="${3:-5}" 

INFILE_NO_EXT="${INFILE%.c}" # remove ".c" INFILE
OUTPUT_FILE="${INFILE_NO_EXT}_e2e_times.csv" 

# Export variables
export INFILE
export SYSROOT
export NUM_ROUNDS
export OUTPUT_FILE
# Set environment variables
export C_INCLUDE_PATH=/usr/riscv64-linux-gnu/include
export LIBRARY_PATH=/usr/riscv64-linux-gnu/lib
export LD_LIBRARY_PATH="${SYSROOT}/usr/lib:/home/prachi/sysroot-link/usr/lib"

# Ensure the script exits on error
set -e

# Run the build script with SYSROOT passed as an argument
# echo "[✓] Running build script with SYSROOT=$SYSROOT..."
./build.sh $SYSROOT
chmod +x patched_main.elf

# Compile original infile to an elf (baseline)
# echo "[✓] Compiling baseline.elf from $INFILE..."
riscv64-linux-gnu-gcc -o baseline.elf $INFILE
chmod +x baseline.elf

# row headers
echo "Time(s),Executable" > "$OUTPUT_FILE"

for ((i=1; i<=NUM_ROUNDS; i++)); do
    # shielded_elf
    qemu_start_time=$(date +%s.%N)
    qemu-riscv64 -L /usr/riscv64-linux-gnu -E "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" ./patched_main.elf
    qemu_end_time=$(date +%s.%N)
    elapsed_time=$(echo "$qemu_end_time - $qemu_start_time" | bc)
    echo "$elapsed_time,shield_elf" >> "$OUTPUT_FILE"
    
    # baseline_elf
    qemu_start_time=$(date +%s.%N)
    qemu-riscv64 -L /usr/riscv64-linux-gnu -E "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" ./baseline.elf
    qemu_end_time=$(date +%s.%N)
    elapsed_time=$(echo "$qemu_end_time - $qemu_start_time" | bc)
    echo "$elapsed_time,baseline_elf" >> "$OUTPUT_FILE"
done

echo "+++ DONE: saved to $OUTPUT_FILE"
