# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file IRVE's usage of rvsw
#

cmake_minimum_required(VERSION 3.16.3)

set(RVSW_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/irve_rvsw_config.cmake")
add_subdirectory(rvsw)
