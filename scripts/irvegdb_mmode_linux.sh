#!/bin/bash
#On the command line, specify the kernel Image followed by vmlinux
trap "kill 0" EXIT
./irvegdb rvsw/compiled/src/single_file/c/nommulinux.vhex8 $1 &
sleep 1
riscv32-unknown-elf-gdb --command=./irvegdb_mmode_gdb_commands --symbols=$2
