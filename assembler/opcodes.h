#pragma once
// There are only 3 memory-request opcodes
// There are no Immediate jumps
// There are only one Immediate operation - LWI


const std::map<std::string, INSTRUCTION_META> INSTRUCTIONS
{
    // No func opcodes
    { "NOP", {0, OPCODE_NOP} },
    { "HLT", {0, OPCODE_HLT} },

    { "JPR", {1, OPCODE_JPR} },
    { "JPC", {1, OPCODE_JPC} },
    { "JOV", {1, OPCODE_JOV} },
    { "JZD", {1, OPCODE_JZD} },
    { "LPC", {1, OPCODE_LPC} },

    { "INC", {2, OPCODE_INC} },
    { "DEC", {2, OPCODE_DEC} },
    { "LWI", {2, OPCODE_LWI} },
    { "TCP", {2, OPCODE_TCP} },
    { "LWD", {2, OPCODE_LWD} },
    { "SWD", {2, OPCODE_SWD} },
    { "NOT", {2, OPCODE_NOT} },
    { "JRL", {2, OPCODE_JRL} },
    { "JGZ", {2, OPCODE_JGZ} },
    { "JLZ", {2, OPCODE_JLZ} },
    { "JEZ", {2, OPCODE_JEZ} },
    { "JNZ", {2, OPCODE_JNZ} },
    { "MOV", {2, OPCODE_MOV} },

    { "ADD", {3, OPCODE_ADD} },
    { "SUB", {3, OPCODE_SUB} },
    { "ADC", {3, OPCODE_ADC} },
    { "SBB", {3, OPCODE_SBB} },
    { "MUL", {3, OPCODE_MUL} },
    { "DIV", {3, OPCODE_DIV} },
    { "UML", {3, OPCODE_UML} },
    { "UDV", {3, OPCODE_UDV} },
    { "MHL", {3, OPCODE_MHL} },
    { "MLH", {3, OPCODE_MLH} },
    { "MHH", {3, OPCODE_MHH} },
    { "MLL", {3, OPCODE_MLL} },
    { "SLL", {3, OPCODE_SLL} },
    { "SRL", {3, OPCODE_SRL} },
    { "SRA", {3, OPCODE_SRA} },
    { "AND", {3, OPCODE_AND} },
    { "ORR", {3, OPCODE_ORR} },
};

/*
     ============================================================================
     Processor: 16-bit RISC with 8 general-purpose registers
     ----------------------------------------------------------------------------
     Summary:
       - Instruction word width: 16 bits
       - Registers: R0..R7 (3-bit addresses)
       - Opcode field width: 7 bits (bits [15:9])
       - R-type instruction format fits entirely in one 16-bit word.
       - Immediate values (for LWI and similar) are stored in the NEXT memory
         word at address PC+1 and are 16 bits wide.
     ============================================================================

     Bit numbering (instruction[15:0], 15 = MSB):
       [15:9]   opcode  : 7 bits
       [ 8:6]   rs_addr : 3 bits
       [ 5:3]   rt_addr : 3 bits
       [ 2:0]   rd_addr : 3 bits

     Notes:
       - This layout yields exactly 16 bits: 7 + 3 + 3 + 3 = 16.
       - There is no separate 'func' field inside the 16-bit word; each opcode
         entirely identifies the operation (as in current opcode.v).
       - For instructions that require an immediate operand (LWI), the immediate
         is fetched from memory at address (PC + 1) and is a full 16-bit value.
       - The PC increments to point to the next instruction word; when executing
         an instruction that consumes an extra word (LWI), the CPU must read
         the immediate from PC+1 and then advance PC accordingly.
     ============================================================================

     Instruction classes and examples (semantic descriptions):
     ----------------------------------------------------------------------------
     R-type (register-register, all encoded in one 16-bit word):
       Format: [opcode:7][rs:3][rt:3][rd:3]

       Arithmetic:
         ADD  rd, rs, rt   -> rd = rs + rt
         SUB  rd, rs, rt   -> rd = rs - rt
         ADC  rd, rs, rt   -> rd = rs + rt + carry
         SBB  rd, rs, rt   -> rd = rs - rt - carry
         MUL  rd, rs, rt   -> rd = (signed) rs * rt (low WORD_WIDTH bits)
         UML  rd, rs, rt   -> rd = (unsigned) rs * rt (low WORD_WIDTH bits)
         DIV  rd, rs, rt   -> rd = (signed) rs / rt
         UDV  rd, rs, rt   -> rd = (unsigned) rs / rt
         INC  rd, rs       -> rd = rs + 1        (rt field ignored or zero)
         DEC  rd, rs       -> rd = rs - 1        (rt field ignored or zero)

       Logic:
         AND  rd, rs, rt   -> rd = rs & rt
         ORR  rd, rs, rt   -> rd = rs | rt
         NOT  rd, rs       -> rd = ~rs            (rt field ignored)
         TCP  rd, rs       -> rd = ~rs + 1        (two's complement)

       Shifts:
         SLL  rd, rs, rt   -> rd = rs << (rt & shift_mask)  (rt used as shift amount)
         SRL  rd, rs, rt   -> rd = rs >> (rt & shift_mask)  (logical)
         SRA  rd, rs, rt   -> rd = arithmetic_shift_right(rs, rt)

       Data movement / memory (R-type):
 
         rd - destination
         rs - source
 
         MOV  rd, rs       -> rd = rs
         LWD  rd, rs       -> rd = mem[ rs ]        (rs holds memory address)
         SWD  rt, rs       -> mem[ rt ] = rs        (rt holds memory address)
         MVH  rd, rs       -> rd[15:8] = rs[15:8]
         MVL  rd, rs       -> rd[7:0]  = rs[7:0]
         JRL  rd, rs       -> { PC+1 -> rd; PC := rs }  link-and-jump

       Jumps / branches (R-type; use register value(s) for condition/target):

         rs - jump target
         rt - jump condition

         JPR  rs       -> PC := rs
         JIR  rs       -> if input_ready then PC : = rs
         JGZ  rs, rt   -> if $signed(rt) > 0 then PC : = rs
         JLZ  rs, rt   -> if $signed(rt) < 0 then PC : = rs
         JEZ  rs, rt   -> if rt == 0 then PC : = rs
         JNZ  rs, rt   -> if rt != 0 then PC : = rs
         JPC  rs       -> if carry_flag then PC : = rs
         JOV  rs       -> if overflow_flag then PC : = rs
         JZD  rs       -> if zerodiv_flag then PC : = rs
     
     ----------------------------------------------------------------------------
     Immediate / special instructions (consume the next memory word):
       These opcodes occupy one 16-bit instruction word; if they require an
       immediate value, that immediate is stored in the NEXT memory word (PC+1).

       NOP (no operation):
         Encoding: opcode = OPCODE_NOP (rest fields ignored)
         Semantics: do nothing; PC := PC + 1

       LWI rd, immediate:
         Encoding (instruction word): [OPCODE_LWI][rs/ignored][rt/ignored][rd]
         Immediate value: 16-bit immediate located at memory[PC + 1]
         Semantics: rd := immediate_from_memory(PC + 1); PC := PC + 2
         Note: CPU must fetch the extra 16-bit word at PC+1 when decoding/executing.

       HLT (halt):
         Encoding: opcode = OPCODE_HLT (rest ignored)
         Semantics: stop processor (enter halted state)

     ----------------------------------------------------------------------------
     Unpacking example (Verilog):
       wire [15:0] instr = instruction_word;
       wire [6:0] opcode = instr[15:9];   // 7 bits
       wire [2:0] rs     = instr[8:6];    // 3 bits
       wire [2:0] rt     = instr[5:3];    // 3 bits
       wire [2:0] rd     = instr[2:0];    // 3 bits

     Immediate fetch for LWI:
       if (opcode == OPCODE_LWI) begin
           immediate = memory[PC + 1];    // full 16-bit immediate
           rd_value   = immediate;
           PC         = PC + 2;
       end else begin
           PC = PC + 1;
       end

     ----------------------------------------------------------------------------
     Registers encoding:
       R0 = 3'b000, R1 = 3'b001, R2 = 3'b010, R3 = 3'b011,
       R4 = 3'b100, R5 = 3'b101, R6 = 3'b110, R7 = 3'b111

     ============================================================================
     Implementation notes / recommendations:
       1) Ensure instruction fetch logic increments PC by 2 when executing
          LWI (or any instruction that consumes an extra word). For typical
          single-cycle fetch/decode, perform an additional memory read at PC+1.
       2) Keep immediate treated as signed or unsigned consistently with ALU
          control (use $signed() in ALU when performing signed operations).
       3) Document in assembler that LWI is a two-word instruction (opcode + imm).
     ============================================================================
 */