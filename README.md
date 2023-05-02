# irve

The Inextensible RISC-V Emulator

"Working our way up to stuff!"

IRVE is intended as a "quick and dirty" emulator capable of running the Linux kernel.

The plan is to use it to better understand the RISC-V architecture (Volume 2 in particular).

Once we do, we can move on to a hardware implementation in SystemVerilog!

Then later on we plan to continue XRVE which will be a much more extensible and powerful emulator written in Rust.

## How to cross compile

### Assembly

Step 0: Install a cross compiler for RISC-V (it is easiest if you make it default to RV32IMA with `--with-arch=rv32ima` and `--with-abi=ilp32`

Step 1: Assemble the file like this: `riscv32-unknown-elf-as -march=rv32ima -mabi=ilp32 path/to/your/assembly_file.s`

Step 2: Objcopy the resulting a.out file to a 32-bit (4-byte) Verilog hex file with `/opt/riscv/bin/riscv32-unknown-elf-objcopy ./a.out -O verilog --verilog-data-width=4 path/to/result.txt`

### C code

TODO
