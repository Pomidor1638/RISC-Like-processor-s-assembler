#include <iostream>


#include "assembler.h"
std::string code = R"(
loop: // 0x0009
    ADD R0, R0, R1 // 1 word
    SUB R5, R2, R0 // 1 word
    JGZ R3, R5     // 1 word
    HLT            // 1 word
START: // 0x0000
    LWI R0, 0x0000 // 2 words 
    LWI R1, 0x0001 // 2 words
    LWI R2, 0x0010 // 2 words 
    LWI R3, loop   // 2 words
    JPR R3         // 1 words
)";

int main(int argc, char* argv[]) {
    
    Assembler asmblr;

    asmblr.assemble(code, true);


    return 0;
}