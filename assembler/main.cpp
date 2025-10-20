#include <iostream>

int main(int argc, char* argv[]) {

    if (argc != 3)
    {
        std::cerr << "Usage: asm.exe <inputfile> <outputfile>" << std::endl;
        return 1;
    }

    std::string  inputfile = argv[1];
    std::string outputfile = argv[2];



    return 0;
}