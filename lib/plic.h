/**
 * @brief   TODO
 * 
 * @copyright
 *  Copyright (C) 2023-2024 John Jekel\n
 *  See the LICENSE file at the root of the project for licensing info.
 * 
 * TODO longer description
 *
*/

#pragma once

#include "common.h"

namespace irve::internal {

/**
 * @brief The IRVE RISC-V PLIC
*/
class Plic {
public:
    Plic();
    ~Plic();

    //26-bit addresses, 32-bit data
    Word read(Word register_address);//NOT const since it could pop from the FIFO
    void write(Word register_address, Word data);

    //TODO other functions to check for MEI and SEI (if they should be set)
};

} // namespace irve::internal
