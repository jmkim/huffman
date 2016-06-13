#include <iostream>
#include <sstream>
#include <string>

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
                printf("[INP] %2x\n", raw);
                printf("[BUF] ");

                for(int i = 0; i < buf_counter; ++i)
                    printf("%2x ", buffer[i]);

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
            printf("%2x ", ch);

        printf("\n");
    }

    {
        fout.clear();
        fout.seekp(0, fout.end);
        std::cout << "Size of result: " << fout.tellp() << std::endl;
    }

    return 0;
}
