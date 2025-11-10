#include <iostream>

#include "assembler.h"
#include "preprocessor.h"


int main(int argc, char* argv[]) {

    /*std::string input_filename;
    std::string output_filename;
    std::vector<instruction_t> instrs;

    if (argc < 3)
    {
        std::cout << "Usage: asm.exe <inputfile> <outputfile>\noptional:\n\t-rom_size\n\t-verbose\n\t-verilog" << std::endl;
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
    Preprocessor prepr;

    qprintf(verbose, 1, "readFile\n%s", input_filename.c_str());
    std::string source_code = readFile(input_filename, verbose);
    

    if (!error_log.has_errors())
        source_code = prepr.preprocess(source_code, verbose);
    if (!error_log.has_errors())
        instrs = asmblr.assemble(source_code, rom_size, verbose);
    if (!error_log.has_errors())
        writeFile(instrs, output_filename, verbose, verilog);

    if (error_log.has_errors())
    {
        std::cerr << error_log.getErrors() << std::endl;
        return EXIT_FAILURE;
    }*/

    Preprocessor prepr;

    std::string source = R"(

    #define DEFINE_MACRO 0x132
    #define DEFINE_WITH_ARGS(reg) reg
    #include "testfile.asm"
    
    #if 1 > 0    
        check if directive
    #elseif
        check else if
    #endif

    #ifdef DEFINE_MACRO
        check ifdef
    #endif
    
    DEFINE_MACRO
    DEFINE_WITH_ARGS(0)
    )";

    std::cout << prepr.preprocess(source) << std::endl;

    return EXIT_SUCCESS;
}