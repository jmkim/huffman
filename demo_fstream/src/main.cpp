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
    while(vin.size() < 5);

    std::cout << "Size of source: " << vin.size() << std::endl;

    std::ostringstream fout (std::ios::binary);

    {
        fout.clear();
        fout.seekp(0, fout.beg);

        const   SizeType        buf_cnt_max     = 8;
        const   SizeType        buf_size_max    = 255;
                CodewordType    buffer          = 5;
                SizeType        buf_cnt_blank   = 3;

        while(vin.size() > 0)
        {
            CodewordPairType    pair            = vin.front();
            vin.erase(vin.begin());
            CodewordType        codeword        = pair.first;
            SizeType            codeword_len    = pair.second;

            while(codeword_len >= buf_cnt_blank)
            {
                {
                    printf("[INP] %2x (%d)\t: ", codeword, codeword_len);
                    print_binary<uint32_t> (codeword, 8);
                    printf("\n");
                }

                buffer <<= buf_cnt_blank;
                buffer += (codeword >> (codeword_len - buf_cnt_blank));
                codeword = codeword % (0x1 << codeword_len - buf_cnt_blank);
                codeword_len -= buf_cnt_blank;

                {
                    printf("[BUF] %02x (%d)\t: ", buffer, buf_cnt_blank);
                    print_binary<uint32_t> (buffer, 8);
                    printf("\n");
                }

                fout.write((char *)&buffer, buf_cnt_max);
                buffer = 0;
                buf_cnt_blank = buf_cnt_max;
            }

            {
                printf("[INP] %2x (%d)\t: ", codeword, codeword_len);
                print_binary<uint32_t> (codeword, 8);
                printf("\n");
            }

            buffer <<= codeword_len;
            buffer += codeword;
            buf_cnt_blank -= codeword_len;
        }
    }

    CodewordPairType    pair            = vin.front();
    vin.erase(vin.begin());
    CodewordType        codeword        = pair.first;
    SizeType            codeword_len    = pair.second;

    while(codeword_len >= buf_cnt_blank)
    {
        {
            printf("[INP] %2x (%d)\t: ", codeword, codeword_len);
            print_binary<uint32_t> (codeword, 8);
            printf("\n");
        }

        buffer <<= buf_cnt_blank;
        buffer += (codeword >> (codeword_len - buf_cnt_blank));
        codeword = codeword % (0x1 << codeword_len - buf_cnt_blank);
        codeword_len -= buf_cnt_blank;

        {
            printf("[BUF] %02x (%d)\t: ", buffer, buf_cnt_blank);
            print_binary<uint32_t> (buffer, 8);
            printf("\n");
        }

        fout.write((char *)&buffer, buf_cnt_max);
        buffer = 0;
        buf_cnt_blank = buf_cnt_max;
    }

    {
        printf("[INP] %2x (%d)\t: ", codeword, codeword_len);
        print_binary<uint32_t> (codeword, 8);
        printf("\n");
    }

    buffer <<= codeword_len;
    buffer += codeword;
    buf_cnt_blank -= codeword_len;

    {
        fout.clear();
        fout.seekp(0, fout.end);
        std::cout << "Size of result: " << fout.tellp() << std::endl;
    }

    return 0;
}
