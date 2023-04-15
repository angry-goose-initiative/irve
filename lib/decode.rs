/* decode.rs 
 * By: John Jekel
 *
 * Code to decode RISC-V instructions 
 *
*/

/* Imports */

use crate::fetch::RawInstruction;
use crate::instruction_handler::InstructionHandler;
use std::collections::HashMap;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

/*pub enum DecodedInstruction {
    RType{rs2: u8, rs1: u8, rd: u8, opcode: DecodedRTypeOpcode},
    IType{imm12: u16, rs1: u8, funct3: u8, rd: u8, opcode: u8},//TODO what about those without imm12?
    SType{imm12: u16, rs2: u8, rs1: u8, opcode: DecodedSTypeOpcode},
    BType{imm13: u16, rs2: u8, rs1: u8, funct3: u8, opcode: u8},//TODO
    UType{imm20: u32, rd: u8, opcode: u8},//TODO
    JType{imm21: u32, rd: u8, opcode: u8},//TODO
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
    Other{funct7: u8, funct3: u8},//Custom
    Invalid
}

pub enum DecodedITypeOpcode {
    Addi,
    Slti,
    Sltiu,
    Xori,
    Ori,
    Andi,
    Slli,
    Srli,
    Srai,
    Other{funct7: u8, funct3: u8},//Custom//TODO this occurs for bad slli/srli/srai 
    Invalid
}

pub enum DecodedSTypeOpcode {
    Sb,
    Sh,
    Sw,
    Other{funct3: u8},//Custom
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
}*/


/*typedef enum {
    LOAD = 0b00000, LOAD_FP = 0b00001, CUSTOM_0 = 0b00010, MISC_MEM = 0b00011, OP_IMM = 0b00100, AUIPC = 0b00101, OP_IMM_32 = 0b00110, B48_0 = 0b00111,
    STORE = 0b01000, STORE_FP = 0b01001, CUSTOM_1 = 0b01010, AMO = 0b01011, OP = 0b01100, LUI = 0b01101, OP_32 = 0b01110, B64 = 0b01111,
    MADD = 0b10000, MSUB = 0b10001, NMSUB = 0b10011, OP_FP = 0b10100, RESERVED_0 = 0b10101, CUSTOM_2 = 0b10110, B48_1 = 0b10111,
    BRANCH = 0b11000, JALR = 0b11001, RESERVED_1 = 0b11010, JAL = 0b11011, SYSTEM = 0b11100, RESERVED_3 = 0b11101, CUSTOM_3 = 0b11110, BGE80 = 0b11111,
} opcode_t;
*/
//TODO
//
//


//TODO new development after this point (this is what we will stick with, not the above)

pub struct Decoder {
    handlers: HashMap<MajorOpcode, Box<dyn InstructionHandler + Send>>,
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
pub enum MajorOpcode {
    Load,
    LoadFp,
    Custom0,
    MiscMem,
    OpImm,
    Auipc,
    OpImm32,
    //48b//TODO figure out how to allow variable length instructions
    Store,
    StoreFp,
    Custom1,
    Amo,
    Op,
    Lui,
    Op32,
    //64b//TODO figure out how to allow variable length instructions
    MAdd,
    MSub,
    NMSub,
    NMAdd,
    OpFp,
    //Reserved,//TODO how to allow use of reserved opcodes?
    Custom2,
    //48b//TODO figure out how to allow variable length instructions
    Branch,
    Jalr,
    //Reserved,//TODO how to allow use of reserved opcodes?
    Jal,
    System,
    //Reserved,//TODO how to allow use of reserved opcodes?
    Custom3,
    //>=80b//TODO figure out how to allow variable length instructions
}

//

/* Associated Functions and Methods */

impl Decoder {
    pub fn new() -> Self {
        Self {
            handlers: HashMap::new(),
        }
    }

    //Returns the previous handler if there was one
    pub fn register_handler(&mut self, handler: impl InstructionHandler + Send + 'static) -> Option<Box<dyn InstructionHandler + Send>> {
        self.handlers.insert(MajorOpcode::Load, Box::new(handler))
    }

    pub fn decode(&self, raw_inst: RawInstruction) -> Result<&mut Box<dyn InstructionHandler + Send>, ()> {
        todo!();//Lookup handler in hashmap and return it
    }
}

//TODO
impl TryFrom<RawInstruction> for MajorOpcode {
    type Error = ();
    fn try_from(raw_inst: RawInstruction) -> Result<Self, Self::Error> {
        match raw_inst {
            RawInstruction::Regular(instruction) => {
                let opcode = (instruction & 0b1111100) >> 2;
                let opcode_lower = opcode & 0b111;
                let opcode_upper = (opcode >> 3) & 0b11;
                match (opcode_upper, opcode_lower) {
                    (0b00, 0b000) => Ok(MajorOpcode::Load),
                    (0b00, 0b001) => Ok(MajorOpcode::LoadFp),
                    (0b00, 0b010) => Ok(MajorOpcode::Custom0),
                    (0b00, 0b011) => Ok(MajorOpcode::MiscMem),
                    (0b00, 0b100) => Ok(MajorOpcode::OpImm),
                    (0b00, 0b101) => Ok(MajorOpcode::Auipc),
                    (0b00, 0b110) => Ok(MajorOpcode::OpImm32),
                    //48b//TODO figure out how to allow variable length instructions
                    (0b01, 0b000) => Ok(MajorOpcode::Store),
                    (0b01, 0b001) => Ok(MajorOpcode::StoreFp),
                    (0b01, 0b010) => Ok(MajorOpcode::Custom1),
                    (0b01, 0b011) => Ok(MajorOpcode::Amo),
                    (0b01, 0b100) => Ok(MajorOpcode::Op),
                    (0b01, 0b101) => Ok(MajorOpcode::Lui),
                    (0b01, 0b110) => Ok(MajorOpcode::Op32),
                    //64b//TODO figure out how to allow variable length instructions
                    (0b10, 0b000) => Ok(MajorOpcode::MAdd),
                    (0b10, 0b001) => Ok(MajorOpcode::MSub),
                    (0b10, 0b010) => Ok(MajorOpcode::NMSub),
                    (0b10, 0b011) => Ok(MajorOpcode::NMAdd),
                    (0b10, 0b100) => Ok(MajorOpcode::OpFp),
                    //Reserved,//TODO how to allow use of reserved opcodes?
                    (0b10, 0b110) => Ok(MajorOpcode::Custom2),
                    //48b//TODO figure out how to allow variable length instructions
                    (0b11, 0b000) => Ok(MajorOpcode::Branch),
                    (0b11, 0b001) => Ok(MajorOpcode::Jalr),
                    //Reserved,//TODO how to allow use of reserved opcodes?
                    (0b11, 0b011) => Ok(MajorOpcode::Jal),
                    (0b11, 0b100) => Ok(MajorOpcode::System),
                    //Reserved,//TODO how to allow use of reserved opcodes?
                    (0b11, 0b110) => Ok(MajorOpcode::Custom3),
                    //>=80b//TODO figure out how to allow variable length instructions
                    _ => Err(())
                }
            },
            RawInstruction::Compressed(_) => {
                return Self::try_from(decompress(raw_inst));
            },
            RawInstruction::Unaligned => {
                return Err(());
            },
            RawInstruction::Fault => {
                return Err(());
            }
        }
    }
}


/* Functions */

fn decompress(instruction: RawInstruction) -> RawInstruction {
    assert!(matches!(instruction, RawInstruction::Compressed(_)));
    todo!();
}

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn major_opcode_try_into_sanity() {
        let inst0 = RawInstruction::Regular(0x00000013);
        assert_eq!(MajorOpcode::try_from(inst0), Ok(MajorOpcode::OpImm));
        let inst1 = RawInstruction::Unaligned;
        assert_eq!(MajorOpcode::try_from(inst1), Err(()));
        let inst2 = RawInstruction::Fault;
        assert_eq!(MajorOpcode::try_from(inst2), Err(()));
    }
}
