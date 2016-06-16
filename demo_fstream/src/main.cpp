#include <iostream>
#include <sstream>
#include <string>
#include <queue>

typedef uint8_t     ByteType;
typedef uint32_t    CodewordType;
typedef size_t      SizeType;

typedef std::istringstream  FileStreamInType;
typedef std::ostringstream  FileStreamOutType;

template<typename T = ByteType>
void
PrintBinary(T src, const size_t & byte_size)
{
    const ByteType  zerobit     = 0;
    const ByteType  nonzerobit  = 1;
          int       bpos        = 0;

    while(src != 0)
    {
        ByteType buffer[byte_size] = { 0, };

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

template<typename T = CodewordType>
void
WriteToFile(FileStreamOutType & fout, T src, const bool & rm_trailing_zerobit = false)
{
    /** Do not use defualt buffer_size (which is 32) */
    const   SizeType        byte_size           = 8;
    const   SizeType        char_size           = sizeof(unsigned char) * byte_size;
    const   SizeType        type_size           = sizeof(T) * byte_size;
    const   SizeType        buffer_size         = (type_size / char_size)
                                                + ((type_size % char_size == 0) ? 0 : 1);
            unsigned char   buffer[buffer_size] = { 0, };

    for(int i = buffer_size - 1; i >= 0; --i)
    {
        buffer[i] = src % (0x1 << byte_size);
        src >>= byte_size;
    }

    SizeType last_pos = buffer_size - 1;

    if(rm_trailing_zerobit == true)
        for(; last_pos > 0 && buffer[last_pos] == 0x0; --last_pos);

    fout.write((char *)&buffer, sizeof(unsigned char) * (last_pos + 1));
}

template<typename T = CodewordType>
void
ReadFromFile(FileStreamInType & fin, T & dest)
{
    const   SizeType        byte_size           = 8;
    const   SizeType        char_size           = sizeof(unsigned char) * byte_size;
    const   SizeType        type_size           = sizeof(T) * byte_size;
    const   SizeType        buffer_size         = (type_size / char_size)
                                                + ((type_size % char_size == 0) ? 0 : 1);
            unsigned char   buffer[buffer_size] = { 0, };

    fin.read((char *)&buffer, sizeof(unsigned char) * buffer_size);

    for(int i = 0; i < buffer_size; ++i)
    {
        dest <<= byte_size;
        dest += buffer[i];
    }
}

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
                    PrintBinary<CodewordType> (codeword, byte_size);
                    printf("\n");
                }

                buffer <<= bufstat_free;
                buffer += (codeword >> (codeword_len - bufstat_free));
                codeword = codeword % (0x1 << codeword_len - bufstat_free);
                codeword_len -= bufstat_free;

                {
                    printf("[BUF] %02x (%lu)\t: ", buffer, bufstat_free);
                    PrintBinary<CodewordType> (buffer, byte_size);
                    printf("\n");
                }

                WriteToFile<CodewordType>(fout, buffer, false);

                buffer = 0;
                bufstat_free = bufstat_max;
            }

            {
                printf("[INP] %02x (%lu)\t: ", codeword, codeword_len);
                PrintBinary<CodewordType> (codeword, byte_size);
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
                PrintBinary<CodewordType> (buffer, byte_size);
                printf("\n");
            }

            WriteToFile<CodewordType>(fout, buffer, true);

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
            PrintBinary<CodewordType> (ch, bufcharr_size);
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

        FileStreamInType fin(fout.str());

        while(! fin.eof())
        {
            ReadFromFile<CodewordType>(fin, buffer);

            {
                printf("[BUF] %02x\t: ", buffer);
                PrintBinary<CodewordType> (buffer, byte_size);
                printf("\n");
            }
        }

        fin.clear();
        fin.seekg(0, fin.end);
        std::cout << "Size of result: " << fin.tellg() << std::endl;
    }

    return 0;
}
