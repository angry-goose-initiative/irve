#!/bin/bash
trap "kill 0" EXIT
./irvegdb rvsw/compiled/sbi/ogsbi/ogsbi.vhex32 $@.vhex32 &
sleep 1
riscv32-unknown-elf-gdb --command=./irvegdb_smode_gdb_commands --symbols=$@.elf
