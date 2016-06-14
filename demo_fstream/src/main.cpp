#include <iostream>
#include <sstream>
#include <string>
#include <queue>

template<typename T = uint8_t>
void
print_binary(T src, const size_t & byte_size)
{
    const uint8_t   zerobit     = 0;
    const uint8_t   nonzerobit  = 1;
          int       bpos        = 0;

    while(src != 0)
    {
        uint8_t buffer[byte_size] = { 0, };

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
    typedef uint32_t            CodewordType;
    typedef size_t              SizeType;

    typedef std::pair<CodewordType, SizeType>   CodewordPairType;
    typedef std::queue<CodewordPairType>        CodewordListType;

    const   SizeType            byte_size       = 32;
    const   SizeType            bufcharr_size   = byte_size / 8;
            CodewordListType    qin;

    do
    {
        CodewordType codeword;
        std::cout << "Codeword: ";
        std::cin  >> codeword;

        SizeType     size;
        std::cout << "Size: ";
        std::cin  >> size;

        qin.push(std::make_pair(codeword, size));
    }
    while(qin.size() < 5);

    std::cout << "Size of source: " << qin.size() << std::endl;

    std::ostringstream fout (std::ios::binary);

    {
        fout.clear();
        fout.seekp(0, fout.beg);

        const   SizeType            bufstat_max     = byte_size;
                SizeType            bufstat_free    = byte_size - 5;
                CodewordType        buffer          = 0x5;

                CodewordType        codeword;
                SizeType            codeword_len;

        while(qin.size() > 0)
        {
            codeword        = qin.front().first;
            codeword_len    = qin.front().second;
                              qin.pop();

            while(codeword_len >= bufstat_free)
            {
                {
                    printf("[INP] %02x (%lu)\t: ", codeword, codeword_len);
                    print_binary<CodewordType> (codeword, byte_size);
                    printf("\n");
                }

                buffer <<= bufstat_free;
                buffer += (codeword >> (codeword_len - bufstat_free));
                codeword = codeword % (0x1 << codeword_len - bufstat_free);
                codeword_len -= bufstat_free;

                {
                    printf("[BUF] %02x (%lu)\t: ", buffer, bufstat_free);
                    print_binary<CodewordType> (buffer, byte_size);
                    printf("\n");
                }

                unsigned char bufcharr[bufcharr_size] = { 0, };

                for(int i = bufcharr_size - 1; i >= 0; --i)
                {
                    bufcharr[i] = buffer % (0x1 << 8);
                    buffer >>= 8;
                }

                fout.write((char *)&bufcharr, sizeof(unsigned char) * bufcharr_size);

                buffer = 0;
                bufstat_free = bufstat_max;
            }

            {
                printf("[INP] %02x (%lu)\t: ", codeword, codeword_len);
                print_binary<CodewordType> (codeword, byte_size);
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
                print_binary<CodewordType> (buffer, byte_size);
                printf("\n");
            }

            unsigned char bufcharr[bufcharr_size] = { 0, };

            for(int i = bufcharr_size - 1; i >= 0; --i)
            {
                bufcharr[i] = buffer % (0x1 << 8);
                buffer >>= 8;
            }

            SizeType last_pos;

            for(last_pos = bufcharr_size - 1; last_pos > 0 && bufcharr[last_pos] == 0x0; --last_pos);

            fout.write((char *)&bufcharr, sizeof(unsigned char) * (last_pos + 1));

            buffer = 0;
            bufstat_free = bufstat_max;
        }
    }

    {
        printf("[FIN] ");

        for(uint8_t ch : fout.str())
            printf("%02x ", ch);

        printf("\n");

        for(uint8_t ch : fout.str())
        {
            printf("      %02x: ", ch);
            print_binary<CodewordType> (ch, bufcharr_size);
            printf("\n");
        }
    }

    {
        fout.clear();
        fout.seekp(0, fout.end);
        std::cout << "Size of result: " << fout.tellp() << std::endl;
    }

    return 0;
}
