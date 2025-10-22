#pragma once

/*
	This instruction package header
	for simplify codereading
*/

//	         common - 16 bits
// 
//	[opcode][15:14] -  2 bits
//	[    rs][13:11] -  3 bits
//	[    rt][10: 8] -  3 bits
//	[    rd][ 7: 5] -  3 bits
//	[  func][ 4: 0] -  5 bits
// 
// instruction = 16'b[opcode][rs][rt][rd][func]

const int      OPCODE_SIZE = 2;
const int REG_ADDRESS_SIZE = 3;
const int        FUNC_SIZE = 5;

const int OPCODE_SHIFT = 14;
const int     RS_SHIFT = 11;
const int     RT_SHIFT = 8;
const int     RD_SHIFT = 5;
const int   FUNC_SHIFT = 0;

// opcodes
const int OPCODE_NOP = 0;
const int OPCODE_R = 1;
const int OPCODE_LWI = 2;
const int OPCODE_HLT = 3;


// funcs
const int FUNC_ADD = 0;
const int FUNC_SUB = 1;

const int FUNC_AND = 2;
const int FUNC_ORR = 3;
const int FUNC_NOT = 4;
const int FUNC_TCP = 5;
const int FUNC_SHL = 6;
const int FUNC_SHR = 7;

const int FUNC_LWD = 8;
const int FUNC_SWD = 9;

const int FUNC_JPR = 10;
const int FUNC_JRL = 11;

const int FUNC_MOV = 12;
const int FUNC_MVH = 13;
const int FUNC_MVL = 14;

const int FUNC_JNE = 15;
const int FUNC_JEQ = 16;
const int FUNC_JGZ = 17;
const int FUNC_JLZ = 18;
const int FUNC_JEZ = 19;
const int FUNC_JNZ = 20;
const int FUNC_JIR = 21;

const int FUNC_NOP = (1 << FUNC_SIZE) - 1;