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

// scary C moment

#include "pack.h"

/*
	I Would optimize with switch statements,
	I wouldn't rather do YandereDev moment 
	for my codebase safety.
*/

struct OPCODE_META
{
	//int instruction_size;	// there is only 1 instruction use 2 words - LWI

	int opcode_args_num;		// MOV rd, rs - 2 args; 
							// ADD rd, rs, rt - 3 args; 
							// JPR rs - 1 arg; 
							// HLT - no args

	int opcode_num; // symbolic num for switch optimisation
	int opcode_code;
	int func_code;
};

const std::string ENTRY_POINT = "START"; // No entry point - No assemble 

#include "opcodes.h"

// 3 bits per register - 8 registers
// R0 is not zero register
// Do with registers all that you want

// I could use some like:
// "R" + i
// string r = "R0"; r[1] += i;
// but I fuck it
 
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

// directives

#include "directives.h"