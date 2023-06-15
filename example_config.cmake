# example_config.cmake
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Example configuration file for rvsw
#

set(RVSW_MMODE_LD_SCRIPT "path/to/linker/script.ld")#Your linker script here
set(RVSW_ENTRY_ADDR "0x00000000")#Your entry address here
set(RVSW_MTVEC_IS_HARDWIRED "Vectored")#Uncomment if not hardwired, else "Direct" if direct, or "Vectored" if vectored
set(RVSW_MTVEC_HARDWIRED_ADDR "0x00000004")#MTVEC hardwired address if applicable
set(RVSW_MARCH "rv32i")#Your -march=[...] here
set(RVSW_MABI "ilp32")#Your -mabi=[...] here

#Your own firmware must link against the SBI static library that is built
set(RVSW_BUILD_SBI "ogsbi")#Uncomment if you want to build an SBI (and specify the name of the one you want to build)

#TODO options for firmware

set(RVSW_BUILD_SMODE_SW "1")#Uncomment if you want to build supervisor mode software too
#TODO other options
