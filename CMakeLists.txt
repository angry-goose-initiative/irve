# CMakeLists.txt
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# CMake build file for rvsw
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
project(rvsw VERSION 0.0.0 LANGUAGES CXX C ASM)
#Needed for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT RVSW_CONFIG_PATH)
    message(FATAL_ERROR "The CMake variable RVSW_CONFIG_PATH must be set to the path of the rvsw config file. Bailing out.")
else()
    message("Using the rvsw config file at ${RVSW_CONFIG_PATH}")
    include(${RVSW_CONFIG_PATH})
endif()

#Add options specified by other RVSW config options to the build command line
set(RVSW_MMODE_COMMON_FLAGS "${RVSW_MMODE_COMMON_FLAGS} -march=${RVSW_MARCH} -mabi=${RVSW_MABI} -T ${RVSW_MMODE_LD_SCRIPT} -DRVSW_MMODE")
set(RVSW_SMODE_COMMON_FLAGS "${RVSW_SMODE_COMMON_FLAGS} -march=${RVSW_SMODE_MARCH} -mabi=${RVSW_SMODE_MABI} -T ${RVSW_SMODE_LD_SCRIPT} -DRVSW_SMODE")
string(REPLACE " " ";" RVSW_MMODE_COMMON_FLAGS "${RVSW_MMODE_COMMON_FLAGS}")#Convert to list
string(REPLACE " " ";" RVSW_SMODE_COMMON_FLAGS "${RVSW_SMODE_COMMON_FLAGS}")#Convert to list

set(CMAKE_ASM_FLAGS "${RVSW_COMMON_FLAGS}")
set(CMAKE_C_FLAGS "${RVSW_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${RVSW_COMMON_FLAGS} -std=gnu++20 -fno-use-cxa-atexit")

set(CMAKE_ASM_FLAGS_DEBUG "${RVSW_COMMON_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${RVSW_COMMON_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "${RVSW_COMMON_FLAGS_DEBUG}")

set(CMAKE_ASM_FLAGS_RELEASE "${RVSW_COMMON_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${RVSW_COMMON_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELEASE "${RVSW_COMMON_FLAGS_RELEASE}")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

#add_subdirectory(firm)
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(sbi)
add_subdirectory(src)
