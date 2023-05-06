# irve

The Inextensible RISC-V Emulator

"Working our way up to stuff!"

IRVE is intended as a "quick and dirty" emulator capable of running the Linux kernel.

The plan is to use it to better understand the RISC-V architecture (Volume 2 in particular).

Once we do, we can move on to a hardware implementation in SystemVerilog!

Then later on we plan to continue XRVE which will be a much more extensible and powerful emulator written in Rust.

## How to cross compile

Step 0: Install a cross compiler for RISC-V (it is easiest if you make it default to RV32IMA with `--with-arch=rv32ima` and `--with-abi=ilp32`

### Assembly

Step 1: Assemble the file like this: `riscv32-unknown-elf-gcc -march=rv32ima -mabi=ilp32 -ffreestanding -nostartfiles -static-libgcc -lgcc -mstrict-align -T path/to/irve.ld path/to/your/assembly_file.s`

BE CAREFUL IF YOU USE THIS COMMAND: `riscv32-unknown-elf-as -march=rv32ima -mabi=ilp32 path/to/your/assembly_file.s`
It does not link the assembly, so references (like to symbols or jump targets) may be unresolved.

### C code

Step 1: Do `riscv32-unknown-elf-gcc -march=rv32ima -mabi=ilp32 -nostartfiles -static -static-libgcc --specs=nosys.specs -lc -lgcc -mstrict-align -T path/to/irve.ld path/to/crt0.s path/to/your/remaining/c/files/here`

NOTE: Software floating point, other missing CPU features will be emulated by libgcc (you won't have to do anything)!

NOTE: The C standard library will be available, BUT since we are using --specs=nosys.specs, you will have to implement certain "system calls" for it in order for ex. `printf` to work.

If we used --specs=nano.specs or others, more would be implemented for us, but Newlib (the c standard library implementation for embedded systems) dosn't know how to use IRVE_DEBUG_ADDR

Note: To implement these functions, this may come in handy: https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib#implementing-newlib

#### How I was compile C code before I thought of the possiblity of having a C library as a possiblity:

`riscv32-unknown-elf-gcc -march=rv32ima -mabi=ilp32 -ffreestanding -nostartfiles -static-libgcc -lgcc -mstrict-align -T path/to/irve.ld path/to/crt0.s path/to/your/remaining/c/files/here`

### End of both

Step 2: Objcopy the resulting a.out file to a 32-bit (4-byte) Verilog hex file with `/opt/riscv/bin/riscv32-unknown-elf-objcopy ./a.out -O verilog --verilog-data-width=4 path/to/result.txt`

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
