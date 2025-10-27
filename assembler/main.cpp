#include <iostream>


#include "assembler.hpp"


int main(int argc, char* argv[]) {

    std::string input_filename;
    std::string output_filename;
    std::vector<instruction_t> instrs;

    if (argc < 3)
    {
        std::cout << "Usage: asm.exe <inputfile> <outputfile> (optional: -rom_size, -verbose, -verilog)" << std::endl;
        return EXIT_FAILURE;
    }

    input_filename = argv[1];
    output_filename = argv[2];

    bool verilog = false;
    bool verbose = false;
    int rom_size = 16384;

    bool bad_param = false;

    for (int i = 3; i < argc; i++)
    {
        std::string str = argv[i];
        if (str == "-rom_size")
        {
            rom_size = std::stoi(argv[i + 1]);
            i++;
        }
        else if (str == "-verbose")
        {
            verbose = true;
        }
        else if (str == "-verilog")
        {
            verilog = true;
        }
        else
        {
            std::cout << "Unexcepted parameter " << str << std::endl;
            bad_param = true;
        }
    }

    if (bad_param) 
    {
        std::cout << "You can only use -rom_size, -verbose, -verilog" << std::endl;
        return EXIT_FAILURE;
    }

    Assembler asmblr;
    qprintf(verbose, 1, "readFile\n%s", input_filename.c_str());
    std::string source_code = asmblr.readFile(input_filename);

    if (asmblr.is_ok())
        instrs = asmblr.assemble(source_code, rom_size, verbose);
    if (asmblr.is_ok())
        asmblr.writeFile(instrs, output_filename, verilog);

    if (!asmblr.is_ok())
    {
        std::cerr << asmblr.getError() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}