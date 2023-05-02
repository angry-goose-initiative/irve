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

# Licensing

## IRVE code 

See the LICENSE file for more info

## Other code

### xrve

Developed alongside/before/after xrve
Licensed in the same way

### rv32esim

Based in part on JZJ's old rv32esim

I (John Jekel) freely release that old code under the MIT License too!

### jzjcoresoftware

MIT License

Copyright (c) 2020 John Jekel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
