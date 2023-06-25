#!/bin/bash
trap "kill 0" EXIT
./irvegdb $@.vhex32 &
sleep 1
riscv32-unknown-elf-gdb --command=./irvegdb_mmode_gdb_commands --symbols=$@.elf
