#!/usr/bin/env python3
import sys
from pathlib import Path

def generate_nops_for_size(size_kb):
    """Generate C code with NOP instructions to exactly fill a specified size in KB,
       along with a main function that calls the NOP-filled function."""
    target_size_bytes = size_kb * 1024
    num_nops = target_size_bytes // 4 
    
    nops_code = f"""
__attribute__((section(".text")))
__attribute__((used))
__attribute__((noinline))
void fill_{size_kb}kb_with_nops(void) {{
"""
    # NOPs
    for _ in range(num_nops):
        nops_code += "    asm volatile(\"add x0, x0, 0\");\n" #add  t3, t1, t2
    nops_code += "}\n"
    
    # generate main
    nops_code += f"""
int main(void) {{
    fill_{size_kb}kb_with_nops();
    return 0;
}}
"""

    return nops_code


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <size1_kb> [size2_kb ...]")
        print("Example: python generate_files.py 1 2 4 8 16")
        sys.exit(1)

    sources_dir = Path('sources')
    sources_dir.mkdir(exist_ok=True)

    for arg in sys.argv[1:]:
        try:
            size_kb = int(arg)
            if size_kb <= 0:
                raise ValueError("Size must be positive")
            
            filename = f"measure_{size_kb}kb.c"
            filepath = sources_dir / filename
            
            filepath.write_text(generate_nops_for_size(size_kb))
            print(f"Generated {filepath}")
            
        except ValueError as e:
            print(f"Error: Invalid size '{arg}' - skipping")
            continue

if __name__ == "__main__":
    main()
