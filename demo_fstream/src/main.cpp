#include <iostream>
#include <sstream>
#include <string>
#include <vector>

template<typename T = char>
void
print_binary(T src, const size_t & byte_size)
{
    const char  zerobit     = 0;
    const char  nonzerobit  = 1;
          int   bpos        = 0;

    while(src != 0)
    {
        char buffer[byte_size] = { 0, };

        for(bpos = byte_size - 1; bpos >= 0; --bpos)
        {
            buffer[bpos] = ((src % 2 == 0) ? zerobit : nonzerobit);
            src = src >> 1;
        }

        if(bpos < 0)
            bpos = 0;

        for(int i = 0; i < byte_size; ++i)
        {
            if(i != 0 && i % 4 == 0)
                printf(" ");
            printf("%d", buffer[i]);
        }

        //printf("(%d) ", bpos);
        printf(" ");
    }
}

int
main(void)
{
    typedef uint32_t    CodewordType;
    typedef size_t      SizeType;
    typedef std::pair<CodewordType, SizeType>   CodewordPairType;
    typedef std::vector<CodewordPairType>       CodewordListType;

    CodewordListType    vin;

    do
    {
        CodewordType codeword;
        std::cout << "Codeword: ";
        std::cin  >> codeword;

        SizeType     size;
        std::cout << "Size: ";
        std::cin  >> size;

        vin.push_back(std::make_pair(codeword, size));
    }
    while(vin.size() < 10);

    std::cout << "Size of source: " << vin.tellg() << std::endl;

    std::ostringstream fout (std::ios::binary);

    {
        CodewordType    buffer;
        SizeType        buf_counter = 32;

        while(! vin.size > 0)
        {
            CodewordPairType    pair            = vin.peek();
            CodewordType        codeword        = pair.first();
            SizeType            codeword_len    = pair.second();

            if(vin.size() <= 0)
                break;

            buf_counter -= codeword_len;
            if(buf_counter >= 0)
                buffer << codeword_len;
            else
            {
            }

            {
                printf("[INP] %02x\n", raw);
                printf("[BUF] ");

                for(int i = 0; i < buf_counter; ++i)
                    printf("%02x ", buffer[i]);

                printf("\n");
            }

            fout.clear();
            fout.seekp(0, vout.end);
            fout.write(&raw, sizeof(int8_t));
        }

    }

    {
        printf("[FIN] ");

        for(auto ch : fout.str())
            printf("%02x ", ch);

        printf("\n");

        for(auto ch : fout.str())
        {
            printf("      %02x: ", ch);
            print_binary<int> (ch, 8);
        }

        printf("\n");
    }

    {
        fout.clear();
        fout.seekp(0, fout.end);
        std::cout << "Size of result: " << fout.tellp() << std::endl;
    }

    return 0;
}
