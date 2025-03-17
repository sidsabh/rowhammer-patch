SHELL := /bin/bash
ENV_SCRIPT := $(realpath env.sh)

CC = riscv64-tcc
CFLAGS = -O2 -Wall -Wextra
BUILD_DIR = build

SRC = poc.c testbench.c
EXES = $(SRC:%.c=$(BUILD_DIR)/%_rv)

all: $(BUILD_DIR) $(EXES)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%_rv: %.c | $(BUILD_DIR)
	BUILD_MODE=1 bash -c "source $(ENV_SCRIPT) && exec $(CC) $(CFLAGS) -o $@ $<"

run-qemu:
	qemu-system-riscv64 \
	  -machine virt \
	  -cpu rv64 \
	  -m 1G \
	  -device virtio-blk-device,drive=hd \
	  -drive file=debian-rv64/dqib_riscv64-virt/overlay.qcow2,if=none,id=hd \
	  -device virtio-net-device,netdev=net \
	  -netdev user,id=net,hostfwd=tcp::2222-:22 \
	  -bios /usr/lib/riscv64-linux-gnu/opensbi/generic/fw_jump.elf \
	  -kernel /usr/lib/u-boot/qemu-riscv64_smode/uboot.elf \
	  -object rng-random,filename=/dev/urandom,id=rng \
	  -device virtio-rng-device,rng=rng \
	  -append "root=LABEL=rootfs console=ttyS0" \
	  -nographic \
	  -fsdev local,id=fsdev0,path=build,security_model=mapped \
	  -device virtio-9p-device,fsdev=fsdev0,mount_tag=sharedfs

clean:
	rm -rf $(BUILD_DIR)

