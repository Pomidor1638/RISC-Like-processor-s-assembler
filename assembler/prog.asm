// =============================================
// Test Program: Counter for ROM
// Addressing: ALWAYS starts at 0x0000
// Registers:
//   R0 - counter (0..16)
//   R1 - temporary values
//   R2 - comparison result
//   R3 - limit value
//   R4 - condition check
// =============================================

loop:
    ADD R0, R2, R0      // R0++ (increment counter)
    
    SUB R3, R0, R4      // R4 = 16 - R0 (check if counter reached limit)
    JGZ R4, R1          // If R0 < 16, jump back to loop
    
    HLT                 // Halt processor (counter reached 16)


START:
    LWI R0, 0           // Initialize counter to 0
    LWI R1, loop        // Load loop address (assembler calculates)
    LWI R2, 1           // Increment value
    LWI R3, 16          // Loop limit