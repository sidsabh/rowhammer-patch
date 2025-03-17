import sys
import struct
import subprocess

def flip_bit(byte, bit_position):
    """Flips the specified bit (0-7) in a byte."""
    return byte ^ (1 << bit_position)

def get_text_section_info(binary_file):
    """Extracts the .text section VMA and file offset from objdump."""
    try:
        output = subprocess.check_output(["riscv64-linux-gnu-objdump", "-h", binary_file], text=True)
        for line in output.splitlines():
            if ".text" in line:
                parts = line.split()
                vma = int(parts[3], 16)  # VMA
                file_off = int(parts[5], 16)  # File offset
                return vma, file_off
    except Exception as e:
        print(f"Error: Unable to read ELF sections: {e}")
        sys.exit(1)

def vma_to_file_offset(vma_target, vma_text, file_off_text):
    """Converts a VMA address to a file offset."""
    return vma_target - vma_text + file_off_text

def modify_binary(input_file, output_file, vma, bit_position):
    """Modifies a binary by flipping a bit at the correct file offset."""
    # Get text section details
    vma_text, file_off_text = get_text_section_info(input_file)

    # Convert VMA to file offset
    file_offset = vma_to_file_offset(vma, vma_text, file_off_text)

    try:
        with open(input_file, "rb") as f:
            binary_data = bytearray(f.read())

        # Ensure offset is within file size
        if file_offset >= len(binary_data):
            print(f"Error: Converted file offset {hex(file_offset)} is out of range! Max offset: {hex(len(binary_data) - 1)}")
            return

        # Flip the bit at the calculated offset
        original_byte = binary_data[file_offset]
        modified_byte = flip_bit(original_byte, bit_position)
        binary_data[file_offset] = modified_byte

        # Write the modified binary
        with open(output_file, "wb") as f:
            f.write(binary_data)

        print(f"✅ Successfully flipped bit {bit_position} at VMA {hex(vma)} (File Offset: {hex(file_offset)}) in {input_file}")
        print(f"📂 New binary saved as: {output_file}")

    except Exception as e:
        print(f"Error: {e}")

# Example Usage
if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python3 bitflip.py <input_binary> <output_binary> <vma> <bit_position>")
        sys.exit(1)

    input_binary = sys.argv[1]
    output_binary = sys.argv[2]
    vma = int(sys.argv[3], 16)  # Accepts hexadecimal input
    bit_position = int(sys.argv[4])

    if not (0 <= bit_position <= 7):
        print("Error: Bit position must be between 0 and 7.")
        sys.exit(1)

    modify_binary(input_binary, output_binary, vma, bit_position)

