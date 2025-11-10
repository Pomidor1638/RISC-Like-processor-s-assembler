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
    OPCODE_ADD = 0b0000001,  // 1
    OPCODE_SUB = 0b0000010,  // 2
    OPCODE_ADC = 0b0000011,  // 3
    OPCODE_SBB = 0b0000100,  // 4
    OPCODE_MUL = 0b0000101,  // 5
    OPCODE_DIV = 0b0000110,  // 6
    OPCODE_UML = 0b0000111,  // 7
    OPCODE_UDV = 0b0001000,  // 8
    OPCODE_INC = 0b0001001,  // 9
    OPCODE_DEC = 0b0001010,  // 10
    OPCODE_AND = 0b0001011,  // 11
    OPCODE_ORR = 0b0001100,  // 12
    OPCODE_NOT = 0b0001101,  // 13
    OPCODE_TCP = 0b0001110,  // 14
    OPCODE_SLL = 0b0001111,  // 15
    OPCODE_SRL = 0b0010000,  // 16
    OPCODE_SRA = 0b0010001,  // 17
    OPCODE_MHL = 0b0010010,  // 18
    OPCODE_MLH = 0b0010011,  // 19
    OPCODE_MLL = 0b0010100,  // 20
    OPCODE_MHH = 0b0010101,  // 21
    OPCODE_MOV = 0b0010110,  // 22
    OPCODE_JGZ = 0b0011000,  // 24
    OPCODE_JLZ = 0b0011001,  // 25
    OPCODE_JEZ = 0b0011010,  // 26
    OPCODE_JNZ = 0b0011011,  // 27
    OPCODE_JPC = 0b0011101,  // 29
    OPCODE_JOV = 0b0011110,  // 30
    OPCODE_JZD = 0b0011111,  // 31
    OPCODE_JPR = 0b0100000,  // 32
    OPCODE_JRL = 0b0100001,  // 33
    OPCODE_LWD = 0b0100010,  // 34
    OPCODE_SWD = 0b0100011,  // 35
    OPCODE_LPC = 0b0100100,  // 36
    OPCODE_LWI = 0b1111101,  // 125
    OPCODE_HLT = 0b1111111   // 127
};
