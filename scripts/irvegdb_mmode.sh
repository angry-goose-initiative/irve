#!/bin/bash
#Just specify the path to the desired mmode rvsw binary without an extension as an argument
trap "kill 0" EXIT
./irvegdb $@.vhex8 &
sleep 1
riscv32-unknown-elf-gdb --command=./irvegdb_mmode_gdb_commands --symbols=$@.elf
