# test_list.txt
# Copyright (C) 2023-2024 John Jekel
# Copyright (C) 2023 Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# List of all IRVE tests (read in by CMake)
#
# To run a test (from the project root dir):
#   mkdir build
#   cd build
#   cmake ..
#   make -j TESTER_NAME
#       Where TESTER_NAME is one of the following: unit_tester, integration_tester, testfiles_tester
#   ctest -j

####################################################################################################
# Unit Test List (unit_tester)
####################################################################################################

add_unit_test(common_Word)
add_unit_test(common_upow)
add_unit_test(common_ipow)
add_unit_test(cpu_state_CpuState)
add_unit_test(CSR_Csr_init)
add_unit_test(decode_decoded_inst_t)
add_unit_test(decode_decoded_inst_t_invalid)
add_unit_test(logging_irvelog)
add_unit_test(uart_uart_t_sanity)
#add_unit_test(uart_uart_t_init)#FIXME re-enable this once the UART is actually wired into memory

add_unit_test(memory_Memory_user_ram_endianness)
add_unit_test(memory_Memory_user_ram_sign_extending)
add_unit_test(memory_Memory_user_ram_valid_byte_access)
add_unit_test(memory_Memory_valid_debugaddr)
add_unit_test(memory_Memory_valid_ramaddrs_halfwords)
add_unit_test(memory_Memory_valid_ramaddrs_words)
add_unit_test(memory_Memory_invalid_debugaddr)
add_unit_test(memory_Memory_invalid_ramaddrs_misaligned_halfwords)
add_unit_test(memory_Memory_invalid_ramaddrs_misaligned_words)
add_unit_test(memory_Memory_translation_conditions)
add_unit_test(memory_Memory_supervisor_loads_with_translation)

#add_unit_test(memory_Memory_invalid_unmapped_bytes)#TODO Not written yet
#add_unit_test(memory_Memory_invalid_unmapped_halfwords)#TODO Not written yet
#add_unit_test(memory_Memory_invalid_unmapped_words)#TODO Not written yet
#add_unit_test(memory_Memory_invalid_unmapped_misaligned_halfwords)#TODO Not written yet
#add_unit_test(memory_Memory_invalid_unmapped_misaligned_words)#TODO Not written yet

####################################################################################################
# Integration Test List (integration_tester)
####################################################################################################

add_integration_test(about)
#add_integration_test(emulator_t_init)#TODO
add_integration_test(emulator_t_sanity)
add_integration_test(logging)

####################################################################################################
# RVSW Test List (rvsw_verifier and rvsw_parse)
####################################################################################################
#NOTE: May or may not correspond to a particular testcase in rvsw

#rvsw_verifier
add_rvsw_test(jzjcoresoftware_adding2)
add_rvsw_test(jzjcoresoftware_auipctest)
add_rvsw_test(jzjcoresoftware_bneandsubtest)
add_rvsw_test(jzjcoresoftware_callrettest)
add_rvsw_test(jzjcoresoftware_fenceecalltest)
add_rvsw_test(jzjcoresoftware_fibbonaccijal)
add_rvsw_test(jzjcoresoftware_fibbonaccijalr)
add_rvsw_test(jzjcoresoftware_lbutest)
add_rvsw_test(jzjcoresoftware_lhtest)
add_rvsw_test(jzjcoresoftware_lhutest)
add_rvsw_test(jzjcoresoftware_loadbytetest)
add_rvsw_test(jzjcoresoftware_luitest)
add_rvsw_test(jzjcoresoftware_memoryreadtest)
add_rvsw_test(jzjcoresoftware_memorywritetest)
add_rvsw_test(jzjcoresoftware_nop)
add_rvsw_test(jzjcoresoftware_sbtest)
add_rvsw_test(jzjcoresoftware_sbtest2)
add_rvsw_test(jzjcoresoftware_shtest)
add_rvsw_test(jzjcoresoftware_sllisrliblttest)
add_rvsw_test(jzjcoresoftware_sllsrlblttest)
add_rvsw_test(jzjcoresoftware_testingfunctions)
add_rvsw_test(jzjcoresoftware_uncondjumptest)
add_rvsw_test(jzjcoresoftware_uncondjumptest2)
add_rvsw_test(jzjcoresoftware_xoritoggle)

add_rvsw_test(rv32esim)
add_rvsw_test(atomics)

#TODO add more

#rvsw_parse (using CTest to determine if an assertion in RISC-V was triggered (to test C/C++ RISC-V programs))
#C
add_rvsw_parse_test(hello_exceptions src/single_file/c/hello_exceptions)
add_rvsw_parse_test(hello_world src/single_file/c/hello_world)
add_rvsw_parse_test(irve_csr_bringup src/single_file/c/irve_csr_bringup)
add_rvsw_parse_test(irve_debugging_puts_weirdness src/single_file/c/irve_debugging_puts_weirdness)
add_rvsw_parse_test(irve_exception_bringup src/single_file/c/irve_exception_bringup)
add_rvsw_parse_test(irve_stress_test src/single_file/c/irve_stress_test)
add_rvsw_parse_test(nouveau_stress_test src/single_file/c/nouveau_stress_test)
add_rvsw_parse_test(poll_timer_interrupt_mmode src/single_file/c/poll_timer_interrupt_mmode)
add_rvsw_parse_test(poll_timer_mmode src/single_file/c/poll_timer_mmode)
add_rvsw_parse_test(rv32esim src/single_file/c/rv32esim)
add_rvsw_parse_test(rv32im_sanity src/single_file/c/rv32im_sanity)
add_rvsw_parse_test(software_floating_point_fun src/single_file/c/software_floating_point_fun)
add_rvsw_parse_test(timer_interrupt_mmode src/single_file/c/timer_interrupt_mmode)
#C++
add_rvsw_parse_test(cxx_experiments src/single_file/cxx/cxx_experiments)
add_rvsw_parse_test(hello_cxx src/single_file/cxx/hello_cxx)
add_rvsw_parse_test(irve_interrupt_bringup src/single_file/cxx/irve_interrupt_bringup)

#S-Mode rvsw_parse (using CTest to determine if an assertion in RISC-V was triggered (to test C/C++ RISC-V programs))
#C
add_rvsw_smode_parse_test(hello_world src/single_file/c/hello_world_smode)
add_rvsw_smode_parse_test(irve_debugging_puts_weirdness src/single_file/c/irve_debugging_puts_weirdness_smode)
add_rvsw_smode_parse_test(irve_stress_test src/single_file/c/irve_stress_test_smode)
add_rvsw_smode_parse_test(nouveau_stress_test src/single_file/c/nouveau_stress_test_smode)
add_rvsw_smode_parse_test(rv32esim src/single_file/c/rv32esim_smode)
add_rvsw_smode_parse_test(rv32im_sanity src/single_file/c/rv32im_sanity_smode)
add_rvsw_smode_parse_test(software_floating_point_fun src/single_file/c/software_floating_point_fun_smode)
#C++
add_rvsw_smode_parse_test(cxx_experiments src/single_file/cxx/cxx_experiments)
add_rvsw_smode_parse_test(hello_cxx src/single_file/cxx/hello_cxx_smode)
add_rvsw_smode_parse_test(irve_vm_bringup src/single_file/cxx/irve_vm_bringup_smode)
add_rvsw_smode_parse_test(morevm_smode src/single_file/cxx/morevm_smode)

#Ones from cppreference
#M-mode
add_rvsw_parse_test(cppreference_dequeue src/single_file/cxx/cppreference/dequeue)
add_rvsw_parse_test(cppreference_integer_sequence src/single_file/cxx/cppreference/integer_sequence)
add_rvsw_parse_test(cppereference_map src/single_file/cxx/cppreference/map)
add_rvsw_parse_test(cppreference_string src/single_file/cxx/cppreference/string)
add_rvsw_parse_test(cppreference_tuple src/single_file/cxx/cppreference/tuple)
#S-mode
add_rvsw_smode_parse_test(cppreference_dequeue src/single_file/cxx/cppreference/dequeue)
add_rvsw_smode_parse_test(cppreference_integer_sequence src/single_file/cxx/cppreference/integer_sequence)
add_rvsw_smode_parse_test(cppereference_map src/single_file/cxx/cppreference/map)
add_rvsw_smode_parse_test(cppreference_string src/single_file/cxx/cppreference/string)
add_rvsw_smode_parse_test(cppreference_tuple src/single_file/cxx/cppreference/tuple)
