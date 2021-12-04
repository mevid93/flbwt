#include <iostream>
#include "flbwt.hpp"


int main(int argc, char *argv[]) 
{
    /* check that user has provided correct number of arguments */
    if (argc != 3) {
        std::cout << "Wrong number of arguments!" << std::endl;
        std::cout << "How to use:" << std::endl;
        std::cout << "    <program_executable> <input_file> <output_file>" << std::endl;
        return -1;
    }

    /* call the burrows-wheeler transform for the input file */
    bwt_file(argv[1], argv[2]);

    return 0;
}