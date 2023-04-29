/* reg_file.h
 * By Nick Chan
 * 
 * The register file
 * Basically works like an array of 32 reg_t but index 0 will always be 0
 * 
*/

#ifndef REG_FILE_H
#define REG_FILE_H

#include <cstdint>
#include <cassert>
#include "reg.h"

class reg_file_t {
private:
    mutable reg_t regs[32];
public:
    reg_file_t();
    reg_t& operator[](uint8_t i);
    const reg_t& operator[](uint8_t i) const;
};

#endif