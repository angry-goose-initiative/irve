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
*/

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

pub trait RISCVStandardFieldsAccessible {
    fn rd(&self) -> Option<u8>;
    fn rs1(&self) -> Option<u8>;
    fn rs2(&self) -> Option<u8>;
    fn funct3(&self) -> Option<u8>;
    fn funct7(&self) -> Option<u8>;
    fn imm_i(&self) -> Option<u32>;
    fn imm_s(&self) -> Option<u32>;
    fn imm_b(&self) -> Option<u32>;
    fn imm_u(&self) -> Option<u32>;
    fn imm_j(&self) -> Option<u32>;
    fn shamt(&self) -> Option<u8>;
}

/* Associated Functions and Methods */

impl Decoder {
    pub fn new() -> Self {
        Self {
            handlers: HashMap::new(),
        }
    }

    //Returns the previous handler if there was one
    pub fn register_handler(&mut self, handler: impl InstructionHandler + Send + 'static) -> Option<Box<dyn InstructionHandler + Send>> {
        self.handlers.insert(handler.get_major_opcode_handled(), Box::new(handler))
    }

    pub fn decode(&mut self, raw_inst: RawInstruction) -> Result<&mut Box<dyn InstructionHandler + Send>, ()> {
        let major_opcode = MajorOpcode::try_from(&raw_inst)?;
        self.handlers.get_mut(&major_opcode).ok_or(())
    }
}

impl TryFrom<&RawInstruction> for MajorOpcode {
    type Error = ();
    fn try_from(raw_inst: &RawInstruction) -> Result<Self, Self::Error> {
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
                return Self::try_from(&decompress(raw_inst));
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

impl RISCVStandardFieldsAccessible for u32 {
    fn rd(&self) -> Option<u8> {
        Some(((self >> 7) & 0b11111) as u8)
    }

    fn rs1(&self) -> Option<u8> {
        Some(((self >> 15) & 0b11111) as u8)
    }

    fn rs2(&self) -> Option<u8> {
        Some(((self >> 20) & 0b11111) as u8)
    }

    fn funct3(&self) -> Option<u8> {
        Some(((self >> 12) & 0b111) as u8)
    }

    fn funct7(&self) -> Option<u8> {
        Some(((self >> 25) & 0b1111111) as u8)
    }

    fn imm_i(&self) -> Option<u32> {
        Some(((*self as i32) >> 20) as u32)//Sign extension using arithmetic shift with i32
    }

    fn imm_s(&self) -> Option<u32> {
        todo!()
    }

    fn imm_b(&self) -> Option<u32> {
        todo!()
    }

    fn imm_u(&self) -> Option<u32> {
        todo!()
    }

    fn imm_j(&self) -> Option<u32> {
        todo!()
    }

    fn shamt(&self) -> Option<u8> {
        Some(((self >> 20) & 0b11111) as u8)
    }
}

impl RISCVStandardFieldsAccessible for RawInstruction {
    fn rd(&self) -> Option<u8> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.rd(),
            RawInstruction::Compressed(_) => decompress(self).rd(),
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn rs1(&self) -> Option<u8> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.rs1(),
            RawInstruction::Compressed(_) => decompress(self).rs1(),
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn rs2(&self) -> Option<u8> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.rs2(),
            RawInstruction::Compressed(_) => decompress(self).rs2(),
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn funct3(&self) -> Option<u8> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.funct3(),
            RawInstruction::Compressed(_) => decompress(self).funct3(),
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn funct7(&self) -> Option<u8> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.funct7(),
            RawInstruction::Compressed(_) => decompress(self).funct7(),
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn imm_i(&self) -> Option<u32> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.imm_i(),
            RawInstruction::Compressed(_) => todo!(),//TODO Can the compress format even hold an immediate?
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn imm_s(&self) -> Option<u32> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.imm_s(),
            RawInstruction::Compressed(_) => todo!(),//TODO Can the compress format even hold an immediate?
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn imm_b(&self) -> Option<u32> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.imm_b(),
            RawInstruction::Compressed(_) => todo!(),//TODO Can the compress format even hold an immediate?
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn imm_u(&self) -> Option<u32> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.imm_u(),
            RawInstruction::Compressed(_) => todo!(),//TODO Can the compress format even hold an immediate?
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn imm_j(&self) -> Option<u32> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.imm_j(),
            RawInstruction::Compressed(_) => todo!(),//Can the compress format even hold an immediate?
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }

    fn shamt(&self) -> Option<u8> {
        match self {
            RawInstruction::Regular(inst_bits) => inst_bits.shamt(),
            RawInstruction::Compressed(_) => todo!(),//Can the compress format even hold a shamt?
            RawInstruction::Unaligned => None,
            RawInstruction::Fault => None
        }
    }
}

/* Functions */

fn decompress(instruction: &RawInstruction) -> RawInstruction {
    debug_assert!(matches!(instruction, RawInstruction::Compressed(_)));
    todo!();
}

/* Tests */

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn major_opcode_try_into_sanity() {
        let inst0 = RawInstruction::Regular(0x00000013);
        assert_eq!(MajorOpcode::try_from(&inst0), Ok(MajorOpcode::OpImm));
        let inst1 = RawInstruction::Unaligned;
        assert_eq!(MajorOpcode::try_from(&inst1), Err(()));
        let inst2 = RawInstruction::Fault;
        assert_eq!(MajorOpcode::try_from(&inst2), Err(()));
    }

    //TODO add more tests (decompress, but also the field/immediate access functions)
}
