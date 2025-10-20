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
const int FUNC_ADD =  0;
const int FUNC_SUB =  1;
					  
const int FUNC_AND =  2;
const int FUNC_ORR =  3;
const int FUNC_NOT =  4;
const int FUNC_TCP =  5;
const int FUNC_SHL =  6;
const int FUNC_SHR =  7;
					  
const int FUNC_LWD =  8;
const int FUNC_SWD =  9;

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

const int FUNC_NOP = 31;

const std::string ENTRY_POINT = "START";


// There are only 3 memory-request opcodes
// There are no Immediate jumps

const std::map<std::string, std::pair<int, int>> OPCODES
{
	// No func opcodes
	{ "NOP", {OPCODE_NOP, FUNC_NOP} },
	{ "LWI", {OPCODE_LWI, FUNC_NOP} },
	{ "HLT", {OPCODE_HLT, FUNC_NOP} },

	// Arithmetic
	{ "ADD", {OPCODE_R  , FUNC_ADD} },
	{ "SUB", {OPCODE_R  , FUNC_SUB} },
	{ "TCP", {OPCODE_R  , FUNC_TCP} },
	
	// Logical
	{ "AND", {OPCODE_R  , FUNC_AND} },
	{ "ORR", {OPCODE_R  , FUNC_ORR} },
	{ "NOT", {OPCODE_R  , FUNC_NOT} },
	
	// Shifts
	{ "SHL", {OPCODE_R  , FUNC_SHL} },
	{ "SHR", {OPCODE_R  , FUNC_SHR} },
	
	/*
		Memory-request opcodes
		These opcodes automatically return to PC + 1 after call
		eliminating the need for explicit return instructions
	*/
	{ "LWD", {OPCODE_R  , FUNC_LWD} }, // LWD R0, R1 - memory[R0] -> R1, it doesn't write return address to registers
	{ "SWD", {OPCODE_R  , FUNC_SWD} },
	
	// Move (i could use ADD instead MOV, but i lazy to change CPU opcodes)
	{ "MOV", {OPCODE_R  , FUNC_MOV} },
	{ "MVH", {OPCODE_R  , FUNC_MVH} },
	{ "MVL", {OPCODE_R  , FUNC_MVL} },

	// Non-condition jumps
	{ "JPR", {OPCODE_R  , FUNC_JPR} },
	{ "JRL", {OPCODE_R  , FUNC_JRL} }, // this shit load return address to reg

	// Condition-depend jumps
	{ "JNE", {OPCODE_R  , FUNC_JNE} },
	{ "JEQ", {OPCODE_R  , FUNC_JEQ} },
	{ "JGZ", {OPCODE_R  , FUNC_JGZ} },
	{ "JLZ", {OPCODE_R  , FUNC_JLZ} },
	{ "JEZ", {OPCODE_R  , FUNC_JEZ} },
	{ "JNZ", {OPCODE_R  , FUNC_JNZ} },
	{ "JIR", {OPCODE_R  , FUNC_JIR} }

};


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
