#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>


namespace irve {
namespace internal {
namespace disassemble {

/// Contains the instruction format of a RISC-V instruction  *  * This should be self-explanatory.  *  * # Safety  *  * Ensure the enum values are legal when configuring the fields of this struct from the C++ side.  * Otherwise undefined behavior may occur after a call to disassemble().
enum class Format {
    R = 0,
    I = 1,
    S = 2,
    B = 3,
    U = 4,
    J = 5,
};

/// Contains the opcode of a RISC-V instruction  *  * This should be self-explanatory.  *  * # Safety  *  * Ensure the enum values are legal when configuring the fields of this struct from the C++ side.  * Otherwise undefined behavior may occur after a call to disassemble().
enum class Opcode {
    Load = 0,
    LoadFp = 1,
    Custom0 = 2,
    MiscMem = 3,
    OpImm = 4,
    AuiPc = 5,
    OpImm32 = 6,
    B480 = 7,
    Store = 8,
    StoreFp = 9,
    Custom1 = 10,
    Amo = 11,
    Op = 12,
    Lui = 13,
    Op32 = 14,
    B64 = 15,
    MAdd = 16,
    MSub = 17,
    NMSub = 18,
    NMAdd = 19,
    OpFp = 20,
    Reserved0 = 21,
    Custom2 = 22,
    B481 = 23,
    Branch = 24,
    Jalr = 25,
    Reserved1 = 26,
    Jal = 27,
    System = 28,
    Reserved3 = 29,
    Custom3 = 30,
    BGE80 = 31,
};

/// Contains the decoded fields of a RISC-V instruction  *  * This struct is used to pass data between the C++ and Rust sides of the library.  * The fields should be self-explanatory.  *  * # Safety  *  * Ensure the enum values are legal when configuring the fields of this struct from the C++ side.  * Otherwise undefined behavior may occur after a call to disassemble().
struct DecodedInst {
    Format format;
    Opcode opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t rs2;
    uint8_t funct3;
    uint8_t funct5;
    uint8_t funct7;
    uint32_t imm;
};


extern "C" {

/// Disassembles a decoded RISC-V instruction into a human-readable C-style string  *  * # Avoiding Memory Leaks  *  * The returned string is allocated on the heap, so it must be freed by the caller when it is no longer needed.  * This can be done by calling free_disassembly() on the pointer returned by this function.
char *disassemble(const DecodedInst *raw_inst);

/// Frees a string returned by disassemble()  *  * # Safety  *  * This function is unsafe because it takes ownership of a pointer and frees it.  * It is the caller's responsibility to ensure that the pointer is valid and that it was originally returned by disassemble().
void free_disassembly(char *disassembly);

} // extern "C"

} // namespace disassemble
} // namespace internal
} // namespace irve
