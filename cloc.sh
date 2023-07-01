#!/bin/bash

#FIXME this is leaving in some files that shouldn't be left in (why does something similar work for IRVE?)
cloc $(git ls-files) --exclude-lang=JSON  --fullpath --not-match-d="build|src/single_file/asm/jzjcoresoftware|src/single_file/cxx/cppreference"
