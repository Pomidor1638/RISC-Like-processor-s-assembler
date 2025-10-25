#include <iostream>


#include "assembler.h"


int main(int argc, char* argv[]) {
    
    std::string input_filename;
    std::string output_filename;

    if (argc < 3)
    {
        std::cout << "Usage: asm.exe <inputfile> <outputfile>" << std::endl;
        return EXIT_FAILURE;
    }
    
    input_filename = argv[1];
    output_filename = argv[2];

    Assembler asmblr;

    std::string source_code = asmblr.readFile(input_filename);

    if (asmblr.is_ok())
    {
        std::vector<instruction_t> instrs = asmblr.assemble(source_code, 16384, true);
        asmblr.writeFile(instrs, output_filename, true);
    }

    return EXIT_SUCCESS;
}