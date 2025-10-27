#pragma once

/*
	This instruction package header
	for simplify codereading
*/

//	         common - 16 bits
// 
//	[opcode][15:14] -  7 bits
//	[    rs][13:11] -  3 bits
//	[    rt][10: 8] -  3 bits
//	[    rd][ 7: 5] -  3 bits
// 
// instruction = 16'b[opcode][rs][rt][rd]

const int      OPCODE_SIZE = 7;
const int REG_ADDRESS_SIZE = 3;

const int OPCODE_SHIFT = 9;
const int     RD_SHIFT = 6;
const int     RS_SHIFT = 3;
const int     RT_SHIFT = 0;

// opcodes
enum OPCODES
{
    OPCODE_NOP = 0b0000000,  // 0
    OPCODE_ADD,  // 1
    OPCODE_SUB,  // 2
    OPCODE_ADC,  // 3
    OPCODE_SBB,  // 4

    OPCODE_MUL,  // 5
    OPCODE_DIV,  // 6
    OPCODE_UML,  // 7
    OPCODE_UDV,  // 8
    OPCODE_INC,  // 9
    OPCODE_DEC,  // 10

    OPCODE_AND,  // 11
    OPCODE_ORR,  // 12
    OPCODE_NOT,  // 13
    OPCODE_TCP,  // 14

    OPCODE_SLL,  // 15
    OPCODE_SRL,  // 16
    OPCODE_SRA,  // 17

    OPCODE_MHL,  // 18
    OPCODE_MLH,  // 19
    OPCODE_MLL,  // 20
    OPCODE_MHH,  // 21
    OPCODE_MOV,  // 22

    OPCODE_JMP,  // 23
    OPCODE_JGZ,  // 24
    OPCODE_JLZ,  // 25
    OPCODE_JEZ,  // 26
    OPCODE_JNZ,  // 27
    OPCODE_JIR,  // 28
    OPCODE_JPC,  // 29
    OPCODE_JOV,  // 30
    OPCODE_JZD,  // 31
    OPCODE_JMZ,  // 32
    OPCODE_JPR,  // 33
    OPCODE_JRL,  // 34

    OPCODE_LWD,  // 35
    OPCODE_SWD,  // 36

    OPCODE_LWI = 0b1111101,  // 125
    OPCODE_HLT = 0b1111111   // 127
};
