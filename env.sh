#!/bin/bash
export TCCSYS=/usr/riscv64-linux-gnu/lib
export C_INCLUDE_PATH=/usr/riscv64-linux-gnu/include
export LIBRARY_PATH=/usr/riscv64-linux-gnu/lib
export TCC_DIR="$(dirname "$(realpath "$0")")/tinycc"
export PATH="$TCC_DIR:$PATH"
