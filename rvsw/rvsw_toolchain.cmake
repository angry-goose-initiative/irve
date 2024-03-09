# rvsw_toolchain.cmake
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake toolchain file to compile RVSW

# This prevents macOS CMake for doing weird things
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
