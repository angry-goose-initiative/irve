#!/bin/bash
#On the command line, specify the kernel Image followed by vmlinux
trap "kill 0" EXIT
./irvegdb rvsw/compiled/src/single_file/c/nommulinux.vhex8 $1 &
sleep 1
riscv32-unknown-elf-gdb -ex "set architecture riscv:rv32" -ex "target remote :12345" -ex "break *0xC0000000" -ex "continue" --symbols=$2
