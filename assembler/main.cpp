#include <iostream>


#include "assembler.h"


int main(int argc, char* argv[]) {
    
    bool is_ok;
    std::string code = readFile("prog.asm", is_ok);

    if (is_ok)
    {
        Assembler asmblr;
        asmblr.assemble(code, 16384, true);
    }

    return 0;
}