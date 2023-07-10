# dtb.S
# Copyright (C) 2023 John Jekel and Nick Chan
# See the LICENSE file at the root of the project for licensing info.
#
# Simply .incbin-s the provided dtb file.
# 

#include "rvsw_asm.h"
.align 8 #THIS IS VERY IMPORTANT
.global dtb_start
dtb_start:
.incbin RVSW_DTB_PATH
