#include <iostream>
#include <sstream>
#include <string>

template<typename T = char>
void
print_binary(T src, const size_t & byte_size)
{
    const char  zerobit     = 0;
    const char  nonzerobit  = 1;
          int   bpos;

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

        std::cout << "(" << bpos << ")" << std::endl;
    }
}

int
main(void)
{
    std::string sin;

    std::cout << "Source: ";
    std::cin  >> sin;

    std::istringstream fin  (sin, std::ios::binary);
    std::ostringstream fout (std::ios::binary);

    {
        fin.clear();
        fin.seekg(0, fin.end);
        std::cout << "Size of source: " << fin.tellg() << std::endl;
    }

    {
        fin.clear();
        fin.seekg(0, fin.beg);

        uint8_t symbol;
        char    raw;
        char    buffer[100];

        int     buf_counter = 0;

        while(! fin.eof())
        {
            fin.read(&raw, sizeof(int8_t));
            if(fin.eof())
                break;

            buffer[buf_counter++] = raw;

            {
                printf("[INP] %02x\n", raw);
                printf("[BUF] ");

                for(int i = 0; i < buf_counter; ++i)
                    printf("%02x ", buffer[i]);

                printf("\n");
            }

            fout.clear();
            fout.seekp(0, fin.end);
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
            print_binary<char> (ch, 8);
        }
    }

    {
        fout.clear();
        fout.seekp(0, fout.end);
        std::cout << "Size of result: " << fout.tellp() << std::endl;
    }

    return 0;
}
