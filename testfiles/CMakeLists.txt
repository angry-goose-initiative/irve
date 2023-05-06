# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve testfiles
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.21)
project(irve_tesfiles VERSION 0.1 LANGUAGES C ASM)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#https://stackoverflow.com/questions/41361631/optimize-in-cmake-by-default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#Compilers to use
#TODO need to probably add these to the path in the future
set(CMAKE_C_COMPILER /opt/riscv/bin/riscv32-unknown-elf-gcc)
set(CMAKE_ASM_COMPILER /opt/riscv/bin/riscv32-unknown-elf-gcc)
set(OBJCOPY /opt/riscv/bin/riscv32-unknown-elf-objcopy)

#Compile options
set(COMMON_FLAGS "-Wall -Wextra -march=rv32ima -mabi=ilp32 -nostartfiles -static -static-libgcc --specs=nosys.specs -lc -lgcc -mstrict-align -T ${CMAKE_CURRENT_SOURCE_DIR}/src/irve.ld")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")
#set(OBJCOPY_FLAGS "-O verilog --verilog-data-width=4")#TODO this dosn't work for some reason

#Static library targets for C runtime setup and other IRVE features
set(LIBIRVE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/c/crt0.s ${CMAKE_CURRENT_SOURCE_DIR}/src/c/irve_newlib.c)
set(LIBIRVE_NO_NEWLIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/c/crt0_no_newlib.s)
add_library(irve STATIC ${LIBIRVE_SOURCES})
add_library(irve_no_newlib STATIC ${LIBIRVE_NO_NEWLIB_SOURCES})

#Macro for outputing Verilog hex file
macro(add_hex_target BINARY_NAME)
    #TODO use OBJCOPY_FLAGS instead of hardcoding
    #add_custom_command(TARGET ${BINARY_NAME} POST_BUILD COMMAND ${OBJCOPY} ARGS ${OBJCOPY_FLAGS} ${BINARY_NAME} ${BINARY_NAME}.hex)
    add_custom_command(TARGET ${BINARY_NAME} POST_BUILD COMMAND ${OBJCOPY} ARGS -O verilog --verilog-data-width=4 ${BINARY_NAME} ${BINARY_NAME}.hex)
endmacro()

#Test executables

#Assembly files
add_executable(rv32esim src/assembly/rv32esim.s)
add_hex_target(rv32esim)
add_executable(adding2 src/assembly/jzjcoresoftware/adding2.mem.s)
add_hex_target(adding2)

#-I is not needed to compile but is needed for clangd to find the include directory
#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fsanitize=address")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto=auto -fuse-linker-plugin")

#rv32esim Library Targets
#set(RV32ESIM_UNSAFE 0)#Speed boost; may cause out of bounds/etc problems if programs executing on the simulator go out-of-bounds/etc
#set(RV32ESIM_DISABLE_LOGGING 0)#Speed boost (avoid checking if enabled, allows compiler to optimize some things more, etc)
#set(RV32ESIM_C 1)#Support compressed extension

#set(IRVE_SOURCES src/

#configure_file(src/rv32esim/cmake_config.h.in src/rv32esim/cmake_config.h)

#add_executable(irve.bin ${IRVE_SOURCES})

#add_library(rv32esim_shared SHARED ${RV32ESIM_SOURCES})
#add_library(rv32esim_static STATIC ${RV32ESIM_SOURCES})
#target_include_directories(rv32esim_shared PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/src/rv32esim/)
#target_include_directories(rv32esim_static PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/src/rv32esim/)
#set_property(TARGET rv32esim_shared PROPERTY OUTPUT_NAME rv32esim)
#set_property(TARGET rv32esim_static PROPERTY OUTPUT_NAME rv32esim)

#rv32esim Test Code
#set(TEST_SOURCES src/main.c include/rv32esim.h)
#add_executable(rv32esim ${TEST_SOURCES})
#target_include_directories(rv32esim PRIVATE include)
#target_link_libraries(rv32esim rv32esim_shared)
