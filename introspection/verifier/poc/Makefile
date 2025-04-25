SYSROOT := /home/sidsabh/code/security/rowhammer-project/riscv/sysroot
CC := riscv64-linux-gnu-gcc

OUTFILE ?= verified_main.elf  # default output name

CFLAGS := -g -nostdlib -ffreestanding -fno-stack-protector \
          -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/openssl

LIBS := -lcrypto -lc -lgcc
LDFLAGS := --sysroot=$(SYSROOT) -T layout.ld -L$(SYSROOT)/usr/lib

all: $(OUTFILE)  # <- *** depend on OUTFILE target, not shell commands

$(OUTFILE): main.o verify.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

verify.o: template_verifier.c
	$(CC) $(CFLAGS) $(CT_MACROS) -c $< -o $@

clean:
	rm -f *.o *.elf
	rm -f verified_main.elf patched_main.elf

.PHONY: all clean
