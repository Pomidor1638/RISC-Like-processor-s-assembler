#include <iostream>

#include "utils.h"

void testIsLabel() {
    std::vector<std::string> test_cases = {
        "main:",      // true - �������� �����
        "loop1:",     // true - �������� �����  
        ":",          // false - ������ ���������
        ".data:",     // false - ���������� � �����
        "123label:",  // false - ���������� � �����
        "my label:",  // false - �������� ������
        "label",      // false - ��� ���������
        ""            // false - ������ ������
    };

    for (const auto& test : test_cases) {
        std::cout << "isLabel(\"" << test << "\") = "
            << std::boolalpha << isLabel(test) << std::endl;
    }
}

int main(int argc, char* argv[]) {

    /*if (argc != 3)
    {
        std::cerr << "Usage: asm.exe <inputfile> <outputfile>" << std::endl;
        return 1;
    }

    std::string  inputfile = argv[1];
    std::string outputfile = argv[2];*/

    testIsLabel();

    return 0;
}