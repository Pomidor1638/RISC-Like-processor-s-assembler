#include <iostream>

#include "assembler.h"
#include "preprocessor.h"


int main(int argc, char* argv[]) {

    std::string input_filename;
    std::string output_filename;
    std::vector<instruction_t> instrs;

    if (argc < 3)
    {
        std::cout << "Usage: asm.exe <inputfile> <outputfile>\noptional:\n\t-rom_size\n\t-verbose\n\t-verilog\n\t-preprocess_out" << std::endl;
        return EXIT_FAILURE;
    }

    input_filename = argv[1];
    output_filename = argv[2];

    bool verilog = false;
    bool verbose = false;
    bool prep_out = false;
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
        else if (str == "-preprocess_out")
        {
            prep_out = true;
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
    Preprocessor prepr;

    qprintf(verbose, 1, "readFile\n%s", input_filename.c_str());
    std::string source_code;

    readFile(input_filename, source_code, verbose);

    if (!error_log.has_errors())
    {
        source_code = prepr.preprocess(source_code, verbose);
    }
    if (!error_log.has_errors() && prep_out)
    {
        writeFile(output_filename + ".prep", source_code, verbose);
    }
    if (!error_log.has_errors())
    {
        instrs = asmblr.assemble(source_code, rom_size, verbose);
    }
    if (!error_log.has_errors())
    {
        writeFile(instrs, output_filename, verbose, verilog);
    }
    if (error_log.has_errors())
    {
        std::cout << error_log.getErrors() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}