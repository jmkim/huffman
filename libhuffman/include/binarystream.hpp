#ifndef ALGORITHM_BINARYSTREAM_H_

#include <istream>
#include <ostream>

namespace algorithm
{

class BinaryStream
{
public:
    typedef size_t          SizeType;
    static  const uint8_t   zerobit     = 0x0;
    static  const uint8_t   nonzerobit  = 0x1;

    /** Member functions */
    template<
        typename T          = uint32_t,
        typename STREAM_OUT = std::ostream
    >
    static
    void
    Write(STREAM_OUT & fout, T src, const bool & rm_trailing_zerobit = false)
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
            for(; last_pos > 0 && buffer[last_pos] == zerobit; --last_pos);

        fout.write((char *)&buffer, sizeof(unsigned char) * (last_pos + 1));
    }

    template<
        typename T          = uint32_t,
        typename STREAM_IN  = std::istream
    >
    static
    void
    Read(STREAM_IN & fin, T & dest)
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

    template<
        typename T          = uint32_t,
        typename STREAM_OUT = std::ostream
    >
    static
    void
    Print(STREAM_OUT & fout, T src, const size_t & buffer_size)
    {
        const uint8_t print_zerobit     = 0x30;
        const uint8_t print_nonzerobit  = 0x31;

        while(src != zerobit)
        {
            uint8_t buffer[buffer_size] = { 0, };

            for(int bpos = buffer_size - 1; bpos >= 0; --bpos)
            {
                buffer[bpos] = ((src % 2 == 0) ? print_zerobit : print_nonzerobit);
                src >>= 0x1;
            }

            for(int i = 0; i < buffer_size; ++i)
            {
                if(i != 0 && i % 4 == 0)
                    fout.write(" ", sizeof(char));

                fout.write((char *)&buffer[i], sizeof(uint8_t));
            }

            fout.write(" ", sizeof(char));
        }
    }
};

} /** ns: algorithm */

#endif /** ! ALGORITHM_BINARYSTREAM_H_ */
