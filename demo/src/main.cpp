#include <iostream>
#include <fstream>
#include "huffman.hpp"

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

    std::string f_name = argv[1];

    std::ifstream f(f_name, std::ios::binary);
    if(! f.is_open())
    {
        fprintf(stderr, "Error: File '%s' not found.\n", f_name.c_str());
        return E_FILE_NOT_FOUND;
    }

    Huffman huffman;
    huffman.CollectRuns(f);
    huffman.PrintAllRuns(stdout);
    huffman.CreateHuffmanTree();
    huffman.PrintHuffmanTree(stdout);

    f.close();

    return E_SUCCESS;
}
