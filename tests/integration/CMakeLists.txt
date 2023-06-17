# CMakeLists.txt
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# CMake configuration file for irve integration tests
#
# Based on CMakeLists.txt from rv32esim
#

#Common options
cmake_minimum_required(VERSION 3.16.3)
include(CTest)

set(
    INTEGRATION_TESTER_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/integration_tester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/init.cpp
)

add_executable(integration_tester ${INTEGRATION_TESTER_SOURCES})
target_include_directories(integration_tester PRIVATE ${PROJECT_SOURCE_DIR}/include/)#To get access to the public libirve headers
#NO internal libirve headers for integration tests (they should only use the public API)
target_link_libraries(integration_tester PRIVATE libirve_object)#TODO or should we make this static or shared instead?
set(INTEGRATION_TEST_LIST "")
macro(add_integration_test TEST_NAME)
    add_test(NAME integration_${TEST_NAME} COMMAND integration_tester ${TEST_NAME})
    set(INTEGRATION_TEST_LIST "${INTEGRATION_TEST_LIST} X(${TEST_NAME})")
    set_property(TEST integration_${TEST_NAME} PROPERTY REQUIRED_FILES "$<TARGET_FILE:integration_tester>")
endmacro()
macro(add_unit_test)
    #Do nothing
endmacro()
macro(add_rvsw_test)
    #Do nothing
endmacro()
macro(add_rvsw_parse_test)
    #Do nothing
endmacro()

include(${CMAKE_CURRENT_SOURCE_DIR}/../test_list.txt)

#Put the list of integration tests in the integration tester file (THIS MUST COME AFTER INCLUDING THE TEST LIST)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/integration_tester.cpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/integration_tester.cpp
    @ONLY
)
