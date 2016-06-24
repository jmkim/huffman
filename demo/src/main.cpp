#include "huffman.hpp"

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

    const   std::string fin_cmp_path    = argv[1],
                        fout_cmp_path   = fin_cmp_path + ".z",
                        fout_dec_path   = fout_cmp_path + ".dec";

    {
        std::ifstream fin(fin_cmp_path, std::ios::binary);
        if(! fin.is_open())
        {
            fprintf(stderr, "Error: File '%s' not found.\n", fin_cmp_path.c_str());
            return E_FILE_NOT_FOUND;
        }

        std::ofstream fout(fout_cmp_path, std::ios::binary);

        Huffman huffman;
        huffman.CompressFile(fin, fout);

        fin.close();
        fout.close();
    }

    {
        std::ifstream fin(fout_cmp_path, std::ios::binary);
        std::ofstream fout(fout_dec_path, std::ios::binary);

        Huffman huffman;
        huffman.DecompressFile(fin, fout);

        fin.close();
        fout.close();
    }

    return E_SUCCESS;
}
