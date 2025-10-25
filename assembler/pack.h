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
const int     RD_SHIFT = 11;
const int     RS_SHIFT = 8;
const int     RT_SHIFT = 5;
const int   FUNC_SHIFT = 0;

// opcodes

enum OPCODES
{

	OPCODE_NOP = 0,
	OPCODE_LWI,
	OPCODE_R,
	OPCODE_HLT,

	OPCODES_COUNT
};

// funcs

enum _FUNCS
{
	FUNC_ADD = 0,
	FUNC_SUB,

	FUNC_AND,
	FUNC_ORR,
	FUNC_NOT,
	FUNC_TCP,
	FUNC_SHL,
	FUNC_SHR,

	FUNC_LWD,
	FUNC_SWD,

	FUNC_JPR,
	FUNC_JRL,

	FUNC_MOV,
	FUNC_CCT,

	FUNC_JGZ,
	FUNC_JLZ,
	FUNC_JEZ,
	FUNC_JNZ,
	FUNC_JIR,

	FUNCS_COUNT,

	FUNC_NOP = (1 << FUNC_SIZE) - 1
};