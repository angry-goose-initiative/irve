#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>


namespace irve {
namespace internal {
namespace disassemble {

struct RawInst {
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
