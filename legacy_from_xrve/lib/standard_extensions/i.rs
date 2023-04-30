/* NAME//TODO
 * By: John Jekel
 *
 * TODO description
 *
*/

/* Imports */

use crate::instruction_handler::InstructionHandler;
use crate::decode::MajorOpcode;
use crate::decode::RISCVStandardFieldsAccessible;
use crate::state::State;
use crate::pmmap::PhysicalMemoryMap;
use crate::fetch::RawInstruction;

/* Constants */

//TODO

/* Macros */

//TODO (also pub(crate) use the_macro statements here too)

/* Static Variables */

//TODO

/* Types */

pub struct LoadInstructionHandler {
    //No state needed
}

pub struct StoreInstructionHandler {
    //No state needed
}

pub struct OpInstructionHandler {
    //No state needed
}

pub struct OpImmInstructionHandler {
    //No state needed
}

//TODO others

/* Associated Functions and Methods */

impl OpInstructionHandler {
    pub fn new() -> Self {
        Self {
            //No state needed
        }
    }
}

impl OpImmInstructionHandler {
    pub fn new() -> Self {
        Self {
            //No state needed
        }
    }
}

impl LoadInstructionHandler {
    pub fn new() -> Self {
        Self {
            //No state needed
        }
    }
}

impl StoreInstructionHandler {
    pub fn new() -> Self {
        Self {
            //No state needed
        }
    }
}

//TODO others

impl InstructionHandler for OpInstructionHandler {
    fn get_major_opcode_handled(&self) -> MajorOpcode {
        MajorOpcode::Op
    }
    fn handle(&self, state: &mut State, _: &mut PhysicalMemoryMap, instruction: RawInstruction) {
        //Get common fields from the instruction
        let rd_index = instruction.rd().expect("instruction should be a valid Op instruction");
        debug_assert!(rd_index < 32, "rd should be less than 32");
        let rs1_index = instruction.rs1().expect("instruction should be a valid Op instruction");
        debug_assert!(rs1_index < 32, "rs1 should be less than 32");
        let rs2_index = instruction.rs2().expect("instruction should be a valid Op instruction");
        debug_assert!(rs2_index < 32, "rs2 should be less than 32");
        let funct3 = instruction.funct3().expect("instruction should be a valid Op instruction");
        let funct7 = instruction.funct7().expect("instruction should be a valid Op instruction");

        //Access rs1 and rs2
        let rs1 = state.get_r(rs1_index);
        let rs2 = state.get_r(rs2_index);

        //Perform the ALU operation
        let result = match (funct7, funct3) {
            (0b0000000, 0b000) => rs1.wrapping_add(rs2),//ADD
            (0b0100000, 0b000) => rs1.wrapping_sub(rs2),//SUB
            (0b0000000, 0b001) => rs1 << (rs2 & 0b11111),//SLL
            (0b0000000, 0b010) => if (rs1 as i32) < (rs2 as i32) { 1 } else { 0 },//SLT
            (0b0000000, 0b011) => if rs1 < rs2 { 1 } else { 0 },//SLTU
            (0b0000000, 0b100) => rs1 ^ rs2,//XOR
            (0b0000000, 0b101) => rs1 >> (rs2 & 0b11111),//SRL
            (0b0100000, 0b101) => ((rs1 as i32) >> (rs2 & 0b11111)) as u32,//SRA
            (0b0000000, 0b110) => rs1 | rs2,//OR
            (0b0000000, 0b111) => rs1 & rs2,//AND
            _ => panic!("Invalid funct3 and funct7 combo for Op")//TODO make this debug_panic
        };

        //Write the result to rd
        state.set_r(rd_index, result);
    }
}

impl InstructionHandler for OpImmInstructionHandler {
    fn get_major_opcode_handled(&self) -> MajorOpcode {
        MajorOpcode::OpImm
    }

    fn handle(&self, state: &mut State, _: &mut PhysicalMemoryMap, instruction: RawInstruction) {
        //Get common fields from the instruction
        let rd_index = instruction.rd().expect("instruction should be a valid OpImm instruction");
        debug_assert!(rd_index < 32, "rd should be less than 32");
        let rs1_index = instruction.rs1().expect("instruction should be a valid OpImm instruction");
        debug_assert!(rs1_index < 32, "rs1 should be less than 32");
        let funct3 = instruction.funct3().expect("instruction should be a valid OpImm instruction");
        let funct7 = instruction.funct7().expect("instruction should be a valid OpImm instruction");
        let imm = instruction.imm_i().expect("instruction should be a valid OpImm instruction");
        let shamt = instruction.shamt().expect("instruction should be a valid OpImm instruction");
        debug_assert!(shamt < 32, "shamt should be less than 32");

        //Access rs1
        let rs1 = state.get_r(rs1_index);

        //Perform the ALU operation
        let result = match funct3 {
            0b000 => rs1.wrapping_add(imm),//ADDI 
            0b001 => rs1 << shamt,//SLLI
            0b010 => if (rs1 as i32) < (imm as i32) { 1 } else { 0 },//SLTI
            0b011 => if rs1 < imm { 1 } else { 0 },//SLTIU
            0b100 => rs1 ^ imm,//XORI
            0b101 => {
                if funct7 == 0b0000000 {
                    rs1 >> shamt//SRLI
                } else if funct7 == 0b0100000 {
                    ((rs1 as i32) >> shamt) as u32//SRAI
                } else {
                    todo!()//How to handle bad opcode?
                }
            },
            0b110 => rs1 | imm,//ORI
            0b111 => rs1 & imm,//ANDI

            _ => panic!("Invalid funct3 for OpImm")//TODO make this debug_panic
        };

        //Write the result to rd
        state.set_r(rd_index, result);
    }
}

impl InstructionHandler for LoadInstructionHandler {
    fn get_major_opcode_handled(&self) -> MajorOpcode {
        MajorOpcode::Load
    }
    fn handle(&self, state: &mut State, pmmap: &mut PhysicalMemoryMap, instruction: RawInstruction) {
        todo!();
    }
}

//TODO others

/* Functions */

//TODO

/* Tests */

//TODO
