/* decode.rs 
 * By: John Jekel
 *
 * Code to decode RISC-V instructions 
 *
*/

/* Imports */

//TODO (include "use" and "mod" here)

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub enum DecodedInstruction {
    RType{funct7: u8, rs2: u8, rs1: u8, funct3: u8, rd: u8, opcode: u8},
    IType{imm12: u16, rs1: u8, funct3: u8, rd: u8, opcode: u8},
    SType{imm12: u16, rs2: u8, rs1: u8, funct3: u8, opcode: u8},
    BType{imm13: u16, rs2: u8, rs1: u8, funct3: u8, opcode: u8},
    UType{imm20: u32, rd: u8, opcode: u8},
    JType{imm21: u32, rd: u8, opcode: u8},
    Other{raw: u32},//Custom
    Invalid
}

pub enum DecodedRTypeOpcode {
    Add,
    Sub,
    Sll,
    Slt,
    Sltu,
    Xor,
    Srl,
    Sra,
    Or,
    And,
    Mul,
    Mulh,
    Mulhsu,
    Mulhu,
    Div,
    Divu,
    Rem,
    Remu,
    Other,//Custom//TODO how to bundle funct7 and funct3 with this?
    Invalid
}

pub enum DecodedInstructionOld {
    Load{imm12: u16, rs1: u8, rd: u8},
    LoadFP,
    Custom0,
    MiscMem,
    OpImm,
    Auipc,
    OpImm32,
    B480,
    Store,
    StoreFP,
    Custom1,
    Amo,
    Op,
    Lui,
    Op32,
    B64,
    MAdd,
    MSub,
    NmSub,
    OpFP,
    Reserved0,
    Custom2,
    B481,
    Branch,
    Jalr,
    Reserved1,
    Jal,
    System,
    Reserved3,
    Custom3,
    Bge80
}


/*typedef enum {
    LOAD = 0b00000, LOAD_FP = 0b00001, CUSTOM_0 = 0b00010, MISC_MEM = 0b00011, OP_IMM = 0b00100, AUIPC = 0b00101, OP_IMM_32 = 0b00110, B48_0 = 0b00111,
    STORE = 0b01000, STORE_FP = 0b01001, CUSTOM_1 = 0b01010, AMO = 0b01011, OP = 0b01100, LUI = 0b01101, OP_32 = 0b01110, B64 = 0b01111,
    MADD = 0b10000, MSUB = 0b10001, NMSUB = 0b10011, OP_FP = 0b10100, RESERVED_0 = 0b10101, CUSTOM_2 = 0b10110, B48_1 = 0b10111,
    BRANCH = 0b11000, JALR = 0b11001, RESERVED_1 = 0b11010, JAL = 0b11011, SYSTEM = 0b11100, RESERVED_3 = 0b11101, CUSTOM_3 = 0b11110, BGE80 = 0b11111,
} opcode_t;
*/
//TODO

/* Associated Functions and Methods */

//TODO

/* Functions */

//TODO
