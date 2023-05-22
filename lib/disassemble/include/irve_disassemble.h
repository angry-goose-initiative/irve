#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>


namespace irve {
namespace internal {
namespace disassemble {

enum class Format {
  R = 0,
  I = 1,
  S = 2,
  B = 3,
  U = 4,
  J = 5,
};

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

struct RawInst {
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

char *disassemble(const RawInst *raw_inst);

void free_disassembly(char *disassembly);

} // extern "C"

} // namespace disassemble
} // namespace internal
} // namespace irve
