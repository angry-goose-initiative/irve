#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>


namespace irve {
namespace internal {
namespace disassemble {


extern "C" {

const char *disassemble(uint32_t raw_inst);

void free_disassembly(char *disassembly);

} // extern "C"

} // namespace disassemble
} // namespace internal
} // namespace irve
