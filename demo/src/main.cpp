#include "../../libhuffman/lib/huffman.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace algorithm;

static enum
{
    E_SUCCESS = 0,
    E_ARGUMENT_NOT_PROVIDED,
    E_FILE_NOT_FOUND,
}
StatusType;

int
main(const int argc, const char * argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s [file name]\n", argv[0]);
        return E_ARGUMENT_NOT_PROVIDED;
    }

    std::string fin_path = argv[1];

    std::ifstream fin(fin_path, std::ios::binary);
    if(! fin.is_open())
    {
        fprintf(stderr, "Error: File '%s' not found.\n", fin_path.c_str());
        return E_FILE_NOT_FOUND;
    }

    Huffman huffman;
    huffman.CompressFile(fin, fin_path, std::string(fin_path + ".z"));

    fin.close();

    return E_SUCCESS;
}
