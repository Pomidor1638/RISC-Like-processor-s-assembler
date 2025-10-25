#pragma once
// There are only 3 memory-request opcodes
// There are no Immediate jumps
// There are only one Immediate operation - LWI

enum ENUM_INSTRUCTIONS
{
    _NOP = 0,
    _HLT,

    _JPR,
    _JIR,

    _LWI,
    _TCP,
    _SHL,
    _SHR,
    _LWD,
    _SWD,
    _NOT,
    _MOV, 
    _JRL,
    _JGZ,
    _JLZ,
    _JEZ,
    _JNZ,

    _CCT,
    _ADD,
    _SUB,
    _AND,
    _ORR,
};

const std::map<std::string, INSTRUCTION_META> INSTRUCTIONS
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

    { "ССT", {2, _CCT, OPCODE_R, FUNC_CCT} },

    // Non-condition jumps
    { "JPR", {1, _JPR, OPCODE_R, FUNC_JPR} },
    { "JRL", {2, _JRL, OPCODE_R, FUNC_JRL} },

    // Condition-depend jumps
    { "JGZ", {2, _JGZ, OPCODE_R, FUNC_JGZ} },
    { "JLZ", {2, _JLZ, OPCODE_R, FUNC_JLZ} },
    { "JEZ", {2, _JEZ, OPCODE_R, FUNC_JEZ} },
    { "JNZ", {2, _JNZ, OPCODE_R, FUNC_JNZ} },
    { "JIR", {1, _JIR, OPCODE_R, FUNC_JIR} },
};

// Процессор: 16-битный RISC с 8 регистрами
// Форматы инструкций:
// R-type: [opcode:2][rs:3][rt:3][rd:3][func:5]
// 
// R-инструкции (opcode=2=10):
/*
ADD rd, rs, rt [OPCODE_R][rs][ rt][rd][FUNC_ADD]  -> 16'b10_xxx_xxx_xxx_00000 // rs + rt -> rd
SUB rd, rs, rt [OPCODE_R][rs][ rt][rd][FUNC_SUB]  -> 16'b10_xxx_xxx_xxx_00001 // rs - rt -> rd
AND rd, rs, rt [OPCODE_R][rs][ rt][rd][FUNC_AND]  -> 16'b10_xxx_xxx_xxx_00010 // rs & rt -> rd
ORR rd, rs, rt [OPCODE_R][rs][ rt][rd][FUNC_ORR]  -> 16'b10_xxx_xxx_xxx_00011 // rs | rt -> rd

NOT rd, rs     [OPCODE_R][rs][___][ rd][FUNC_NOT] -> 16'b10_xxx_xxx_xxx_00100 // ~rs -> rd
TCP rd, rs     [OPCODE_R][rs][___][ rd][FUNC_TCP] -> 16'b10_xxx_xxx_xxx_00101 // ~rs + 1 -> rd
SHL rd, rs     [OPCODE_R][rs][___][ rd][FUNC_SHL] -> 16'b10_xxx_xxx_xxx_00110 // rs << 1 -> rd
SHR rd, rs     [OPCODE_R][rs][___][ rd][FUNC_SHR] -> 16'b10_xxx_xxx_xxx_00111 // rs >> 1 -> rd
LWD rd, rs     [OPCODE_R][rs][___][ rd][FUNC_LWD] -> 16'b10_xxx_xxx_xxx_01000 // mem[rs] -> rd
MOV rd, rs     [OPCODE_R][rs][___][ rd][FUNC_MOV] -> 16'b10_xxx_xxx_xxx_01100 // rs -> rd
MVH rd, rs     [OPCODE_R][rs][___][ rd][FUNC_MVH] -> 16'b10_xxx_xxx_xxx_01101 // rs[15:8] -> rd[15:8]
MVL rd, rs     [OPCODE_R][rs][___][ rd][FUNC_MVL] -> 16'b10_xxx_xxx_xxx_01110 // rs[7:0] -> rd[7:0]
JRL rd, rs     [OPCODE_R][rs][___][ rd][FUNC_JRL] -> 16'b10_xxx_xxx_xxx_01011 // PC+1 -> rd, rs -> PC

SWD rt, rs     [OPCODE_R][rs][ rt][___][FUNC_SWD] -> 16'b10_xxx_xxx_xxx_01001 // rs -> mem[rt]
JGZ rt, rs     [OPCODE_R][rs][ rt][___][FUNC_JGZ] -> 16'b10_xxx_xxx_xxx_01111 // rs > 0 ? rt -> PC
JLZ rt, rs     [OPCODE_R][rs][ rt][___][FUNC_JLZ] -> 16'b10_xxx_xxx_xxx_10000 // rs < 0 ? rt -> PC
JEZ rt, rs     [OPCODE_R][rs][ rt][___][FUNC_JEZ] -> 16'b10_xxx_xxx_xxx_10001 // rs == 0 ? rt -> PC
JNZ rt, rs     [OPCODE_R][rs][ rt][___][FUNC_JNZ] -> 16'b10_xxx_xxx_xxx_10010 // rs != 0 ? rt -> PC

JIR rs         [OPCODE_R][rs][___][___][FUNC_JIR] -> 16'b10_xxx_xxx_xxx_10011 // input_ready ? rs -> PC
JPR rs         [OPCODE_R][rs][___][___][FUNC_JPR] -> 16'b10_xxx_xxx_xxx_01010 // rs -> PC
*/

// Immediate инструкции:
/*
NOP               [OPCODE_NOP][___][___][___][_____] -> 16'b00_xxx_xxx_xxx_xxxxx // нет операции
LWI rd, immediate [OPCODE_LWI][ rd][___][___][_____] -> 16'b01_xxx_xxx_xxx_xxxxx + [immediate:16] // mem[PC+1] -> rd
HLT               [OPCODE_HLT][___][___][___][_____] -> 16'b11_xxx_xxx_xxx_xxxxx // останов процессора
*/

// Регистры: R0(000), R1(001), R2(010), R3(011), R4(100), R5(101), R6(110), R7(111)

// Распаковка инструкции:
// wire [1:0] opcode    = instruction[15:14];    // 2 бита
// wire [2:0] rs_addr   = instruction[13:11];    // 3 бита  
// wire [2:0] rt_addr   = instruction[10:8];     // 3 бита
// wire [2:0] rd_addr   = instruction[7:5];      // 3 бита
// wire [4:0] func      = instruction[4:0];      // 5 бит
