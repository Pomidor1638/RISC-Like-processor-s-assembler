#pragma once

#include <cstdint>

#include <vector>
#include <list>
#include <string>
#include <map>

#include <algorithm>
#include <cctype>

#include <fstream>
#include <sstream>

typedef uint8_t byte;
typedef int address_t;

typedef uint16_t instruction_t;

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
const int     RT_SHIFT =  8;
const int     RD_SHIFT =  5;
const int   FUNC_SHIFT =  0;

// opcodes
const int OPCODE_NOP = 0;
const int OPCODE_R   = 1;
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

const int FUNC_JPR = 14;
const int FUNC_JRL = 15;

const int FUNC_MOV = 16;
const int FUNC_MVH = 17;
const int FUNC_MVL = 18;

const int FUNC_JNE = 19; 
const int FUNC_JEQ = 20;
const int FUNC_JGZ = 21;
const int FUNC_JLZ = 22;
const int FUNC_JEZ = 23;
const int FUNC_JNZ = 24;
const int FUNC_JIR = 25;

const std::string ENTRY_POINT = "START";

const std::map<std::string, int> OPCODES
{
	{ "NOP", OPCODE_NOP },
	{ "LWI", OPCODE_LWI },
	{ "HLT", OPCODE_HLT }
};

// OPCODE_R

const std::map<std::string, int> FUNCS
{

	{ "ADD", FUNC_ADD },
	{ "SUB", FUNC_SUB },
	{ "AND", FUNC_AND },
	{ "ORR", FUNC_ORR },
	{ "NOT", FUNC_NOT },
	{ "TCP", FUNC_TCP },
	{ "SHL", FUNC_SHL },
	{ "SHR", FUNC_SHR },

	{ "LWD", FUNC_LWD },
	{ "SWD", FUNC_SWD },

	{ "JPR", FUNC_JPR },
	{ "JRL", FUNC_JRL },

	{ "MOV", FUNC_MOV },
	{ "MVH", FUNC_MVH },
	{ "MVL", FUNC_MVL },

	{ "JNE", FUNC_JNE },
	{ "JEQ", FUNC_JEQ },
	{ "JGZ", FUNC_JGZ },
	{ "JLZ", FUNC_JLZ },
	{ "JEZ", FUNC_JEZ },
	{ "JNZ", FUNC_JNZ },
	{ "JIR", FUNC_JIR }
};

// REGISTERS

const std::map<std::string, int> REGISTERS
{
	{ "R0", 0 },
	{ "R1", 1 },
	{ "R2", 2 },
	{ "R3", 3 },
	{ "R4", 4 },
	{ "R5", 5 },
	{ "R6", 6 },
	{ "R7", 7 }
};
