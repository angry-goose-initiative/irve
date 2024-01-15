# dtb.S
# Copyright (C) 2023-2024 John Jekel
# See the LICENSE file at the root of the project for licensing info.
#
# Simply .incbin-s the provided dtb file.
# 

#include "rvsw_asm.h"
.section .rodata
.align 3 #THIS IS VERY IMPORTANT (must align to 8 bytes, or 2^3)
.global dtb_start
dtb_start:
.incbin RVSW_DTB_PATH
