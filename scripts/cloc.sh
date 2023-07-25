#!/bin/bash

cloc $(git ls-files) --exclude-lang=JSON,Text  --fullpath --not-match-d="build|rvsw/rvsw/src/single_file/asm/jzjcoresoftware|rvsw/rvsw/src/single_file/cxx/cppreference|tests/riscv_arch_test/spike|tests/riscv_arch_test/spike/env"
