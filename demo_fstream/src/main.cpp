#include <iostream>
#include <sstream>
#include <string>
#include <queue>

#include "binarystream.hpp"

using namespace algorithm;

typedef uint8_t     ByteType;
typedef uint32_t    CodewordType;
typedef size_t      SizeType;

typedef std::istringstream  StreamInType;
typedef std::ostringstream  StreamOutType;

int
main(void)
{
    typedef std::pair<CodewordType, SizeType>   CodewordPairType;
    typedef std::queue<CodewordPairType>        CodewordListType;

    const   SizeType            byte_size       = 32;
    const   SizeType            bufcharr_size   = byte_size / 8;
            CodewordListType    fin;

    do
    {
        CodewordType codeword;
        std::cout << "Codeword: ";
        std::cin  >> codeword;

        SizeType     size;
        std::cout << "Size: ";
        std::cin  >> size;

        fin.push(std::make_pair(codeword, size));
    }
    while(fin.size() < 5);

    std::cout << "Size of source: " << fin.size() << std::endl;

    std::ostringstream fout (std::ios::binary);

    {
        fout.clear();
        fout.seekp(0, fout.beg);

        const   SizeType            bufstat_max     = byte_size;
                SizeType            bufstat_free    = byte_size - 5;
                CodewordType        buffer          = 0x5;

                CodewordType        codeword;
                SizeType            codeword_len;

        while(fin.size() > 0)
        {
            codeword        = fin.front().first;
            codeword_len    = fin.front().second;
                              fin.pop();

            while(codeword_len >= bufstat_free)
            {
                {
                    printf("[INP] %02x (%lu)\t: ", codeword, codeword_len);
                    BinaryStream::Print<CodewordType>(std::cout, codeword, byte_size);
                    printf("\n");
                }

                buffer <<= bufstat_free;
                buffer += (codeword >> (codeword_len - bufstat_free));
                codeword = codeword % (0x1 << codeword_len - bufstat_free);
                codeword_len -= bufstat_free;

                {
                    printf("[BUF] %02x (%lu)\t: ", buffer, bufstat_free);
                    BinaryStream::Print<CodewordType>(std::cout, buffer, byte_size);
                    printf("\n");
                }

                BinaryStream::Write<CodewordType>(fout, buffer, false);

                buffer = 0;
                bufstat_free = bufstat_max;
            }

            {
                printf("[INP] %02x (%lu)\t: ", codeword, codeword_len);
                BinaryStream::Print<CodewordType>(std::cout, codeword, byte_size);
                printf("\n");
            }

            buffer <<= codeword_len;
            buffer += codeword;
            bufstat_free -= codeword_len;
        }

        if(bufstat_free != bufstat_max)
        {
            buffer <<= bufstat_free;

            {
                printf("[BUF] %02x (%lu)\t: ", buffer, bufstat_free);
                BinaryStream::Print<CodewordType>(std::cout, buffer, byte_size);
                printf("\n");
            }

            BinaryStream::Write<CodewordType>(fout, buffer, true);

            buffer = 0;
            bufstat_free = bufstat_max;
        }
    }

    {
        printf("[FIN] ");

        for(ByteType ch : fout.str())
            printf("%02x ", ch);

        printf("\n");

        for(ByteType ch : fout.str())
        {
            printf("      %02x: ", ch);
            BinaryStream::Print<CodewordType>(std::cout, ch, bufcharr_size);
            printf("\n");
        }
    }

    {
        fout.clear();
        fout.seekp(0, fout.end);
        std::cout << "Size of result: " << fout.tellp() << std::endl;
    }

    {
        CodewordType buffer;

        StreamInType fin(fout.str());

        while(! fin.eof())
        {
            BinaryStream::Read<CodewordType>(fin, buffer);

            {
                printf("[BUF] %02x\t: ", buffer);
                BinaryStream::Print<CodewordType>(std::cout, buffer, byte_size);
                printf("\n");
            }
        }

        fin.clear();
        fin.seekg(0, fin.end);
        std::cout << "Size of result: " << fin.tellg() << std::endl;
    }

    return 0;
}
