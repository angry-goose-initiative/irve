#!/bin/bash
#Just specify the path to the desired smode rvsw binary without an extension as an argument
trap "kill 0" EXIT
./irvegdb rvsw/compiled/sbi/ogsbi/ogsbi.vhex8 $@.vhex8 &
sleep 1
riscv32-unknown-elf-gdb -ex "set architecture riscv:rv32" -ex "target remote :12345" -ex "break *0x80000000" -ex "continue" --symbols=$@.elf
