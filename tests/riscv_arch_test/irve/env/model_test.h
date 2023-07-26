/**
 * @file    model_test.h
 * @brief   TODO
 * 
 * @copyright Copyright (C) 2023 John Jekel and Nick Chan
 * See the LICENSE file at the root of the project for licensing info.
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2019, incoresemi
 * Copyright (c) 2019, IIT Madras
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * TODO longer description
 *
*/

#ifndef IRVE_MODEL_TEST_H
#define IRVE_MODEL_TEST_H

/* ------------------------------------------------------------------------------------------------
 * Constants/Defines
 * --------------------------------------------------------------------------------------------- */

#define RVMODEL_HALT                                              \
    li x1, 'H'; \
    sw x1, -1(zero); \
    li x1, 'A'; \
    sw x1, -1(zero); \
    li x1, 'L'; \
    sw x1, -1(zero); \
    li x1, 'T'; \
    sw x1, -1(zero); \
    li x1, '\n'; \
    sw x1, -1(zero); \
    .insn r CUSTOM_0, 0, 0, zero, zero, zero;

#define RVMODEL_BOOT \
    li x1, 'B'; \
    sw x1, -1(zero); \
    li x1, 'O'; \
    sw x1, -1(zero); \
    li x1, 'O'; \
    sw x1, -1(zero); \
    li x1, 'T'; \
    sw x1, -1(zero); \
    li x1, '\n'; \
    sw x1, -1(zero);

#define RVMODEL_DATA_BEGIN                                              \
    .data; \
    .align 4;\
    .global irve_begin_signature; irve_begin_signature:

#define RVMODEL_DATA_END                                                      \
  .align 4;\
  .global irve_end_signature; irve_end_signature:

#define RVMODEL_IO_INIT
#define RVMODEL_IO_WRITE_STR(_R, _STR)
#define RVMODEL_IO_CHECK()
#define RVMODEL_IO_ASSERT_GPR_EQ(_S, _R, _I)
#define RVMODEL_IO_ASSERT_SFPR_EQ(_F, _R, _I)
#define RVMODEL_IO_ASSERT_DFPR_EQ(_D, _R, _I)

#endif//IRVE_MODEL_TEST_H
