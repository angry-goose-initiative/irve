/**
 * @file    workaround.cpp
 * @brief   Temporary workarounds to get C++ working in RVSW
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
*/

/* ------------------------------------------------------------------------------------------------
 * Global Variables
 * --------------------------------------------------------------------------------------------- */

void* __dso_handle = nullptr;//FIXME we shouldn't have to do this
