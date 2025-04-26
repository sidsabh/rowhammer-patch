#Can pass your sysroot as an argument
SYSROOT ?= /home/prachi/sysroot-link#/home/sidsabh/code/security/rowhammer-project/riscv/sysroot
CC := riscv64-linux-gnu-gcc

CFLAGS := -g -nostdlib -ffreestanding -fno-stack-protector \
          -I$(SYSROOT)/usr/include -I$(SYSROOT)/usr/include/openssl

LIBS := -lcrypto -lc -lgcc -lelf -lz
LDFLAGS := --sysroot=$(SYSROOT) -T layout.ld -L$(SYSROOT)/usr/lib

# Set defaults
INFILE ?= tests/simple.c # main.c
FINAL_BUILD ?= 0
CT_MACROS ?=

# Pick output binary depending on FINAL_BUILD
ifeq ($(FINAL_BUILD),1)
    OUTFILE := patched_main.elf
else
    OUTFILE := verified_main.elf
endif

OBJFILE := $(basename $(INFILE)).o

all: $(OUTFILE)

$(OUTFILE): $(OBJFILE) verify.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJFILE): $(INFILE)
	$(CC) $(CFLAGS) -c $< -o $@

verify.o: template_verifier.c
	$(CC) $(CFLAGS) $(CT_MACROS) -c $< -o $@

clean:
	rm -f *.o *.elf tests/*.o
	rm -f verified_main.elf patched_main.elf
