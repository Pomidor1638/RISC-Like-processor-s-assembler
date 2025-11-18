#define RAM_START   16384
#define RAM_SIZE    16384

#define STACK_START 32763
#define BP_PTR      32764
#define SP_PTR      32765
#define BP_BUF_PTR  32766
#define RET_BUF_PTR 32767

#macro STACK_INIT
    LWI R7, STACK_START
    LWI R6, SP_PTR
    SWD R6, R7
    LWI R6, BP_PTR
    SWD R6, R7  // »нициализируем BP тоже
#endmacro

#macro PUSH reg
    LWI R7, SP_PTR      // R7 <- адрес SP_PTR
    LWD R6, R7          // R6 <- mem[R7] (R6 <- текущий SP)
    
    DEC R6, R6          // R6-- (SP--)
    SWD R6, reg         // mem[R6] <- reg

    SWD R7, R6          // mem[SP_PTR] <- R6 (обновл€ем SP)
#endmacro

#macro POP reg
    LWI R7, SP_PTR      // R7 <- SP_PTR
    LWD R6, R7          // R6 <- mem[R7] (R6 <- SP)
    
    LWD reg, R6         // reg <- mem[R6] (reg <- mem[SP])
    
    INC R6, R6          // R6++ (SP++)
    
    SWD R7, R6          // mem[SP_PTR] <- R6 (SP <- R6)
#endmacro
