#pragma once
// There are only 3 memory-request opcodes
// There are no Immediate jumps
// There are only one Immediate operation - LWI


const int _NOP = 0;
const int _LWI = 1;
const int _HLT = 2;
const int _ADD = 3;
const int _SUB = 4;
const int _TCP = 5;
const int _AND = 6;
const int _ORR = 7;
const int _NOT = 8;
const int _SHL = 9;
const int _SHR = 10;
const int _LWD = 11;
const int _SWD = 12;
const int _MOV = 13;
const int _MVH = 14;
const int _MVL = 15;
const int _JPR = 16;
const int _JRL = 17;
const int _JNE = 18;
const int _JEQ = 19;
const int _JGZ = 20;
const int _JLZ = 21;
const int _JEZ = 22;
const int _JNZ = 23;
const int _JIR = 24;

const std::map<std::string, OPCODE_META> OPCODES
{
    // No func opcodes
    { "NOP", {0, _NOP, OPCODE_NOP, FUNC_NOP} },
    { "LWI", {2, _LWI, OPCODE_LWI, FUNC_NOP} },
    { "HLT", {0, _HLT, OPCODE_HLT, FUNC_NOP} },

    // Arithmetic
    { "ADD", {3, _ADD, OPCODE_R, FUNC_ADD} },
    { "SUB", {3, _SUB, OPCODE_R, FUNC_SUB} },
    { "TCP", {2, _TCP, OPCODE_R, FUNC_TCP} },

    // Logical
    { "AND", {3, _AND, OPCODE_R, FUNC_AND} },
    { "ORR", {3, _ORR, OPCODE_R, FUNC_ORR} },
    { "NOT", {2, _NOT, OPCODE_R, FUNC_NOT} },

    // Shifts
    { "SHL", {2, _SHL, OPCODE_R, FUNC_SHL} },
    { "SHR", {2, _SHR, OPCODE_R, FUNC_SHR} },

    // Memory
    { "LWD", {2, _LWD, OPCODE_R, FUNC_LWD} },
    { "SWD", {2, _SWD, OPCODE_R, FUNC_SWD} },

    // Move
    { "MOV", {2, _MOV, OPCODE_R, FUNC_MOV} },
    { "MVH", {2, _MVH, OPCODE_R, FUNC_MVH} },
    { "MVL", {2, _MVL, OPCODE_R, FUNC_MVL} },

    // Non-condition jumps
    { "JPR", {1, _JPR, OPCODE_R, FUNC_JPR} },
    { "JRL", {2, _JRL, OPCODE_R, FUNC_JRL} },

    // Condition-depend jumps
    { "JNE", {2, _JNE, OPCODE_R, FUNC_JNE} },
    { "JEQ", {2, _JEQ, OPCODE_R, FUNC_JEQ} },
    { "JGZ", {2, _JGZ, OPCODE_R, FUNC_JGZ} },
    { "JLZ", {2, _JLZ, OPCODE_R, FUNC_JLZ} },
    { "JEZ", {2, _JEZ, OPCODE_R, FUNC_JEZ} },
    { "JNZ", {2, _JNZ, OPCODE_R, FUNC_JNZ} },
    { "JIR", {1, _JIR, OPCODE_R, FUNC_JIR} },
};