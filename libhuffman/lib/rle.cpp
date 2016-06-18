#include "rle.hpp"
#include "binarystream.hpp"

using namespace algorithm;

void
RLE
::Encode(StreamInType & fin, StreamOutType & fout)
{

    fin.clear();
    fin.seekg(0, fin.beg);

    if(! fin.eof())
    {
        SymbolType  symbol, next_symbol;
        SizeType    run_len = 1;

        BinaryStream::Read<SymbolType>(fin, symbol);

        while(! fin.eof())
        {
            BinaryStream::Read<SymbolType>(fin, next_symbol);
            if(fin.eof())
                break;

            if(symbol == next_symbol)
                ++run_len;
            else
            {
                BinaryStream::Write<SymbolType>(fout, symbol);
                BinaryStream::Write<SizeType>(fout, run_len);

                run_len = 1;
            }

            symbol = next_symbol;
        }

        BinaryStream::Write<SymbolType>(fout, symbol);
        BinaryStream::Write<SizeType>(fout, run_len);
    }
}

void
RLE
::Decode(StreamInType & fin, StreamOutType & fout)
{
    fin.clear();
    fin.seekg(0, fin.beg);

    while(! fin.eof())
    {
        SymbolType  symbol;
        SizeType    run_len;

        BinaryStream::Read<SymbolType>(fin, symbol);
        if(fin.eof())
            break;

        BinaryStream::Read<SizeType>(fin, run_len);

        for(int i = 0; i < int(run_len); ++i)
            BinaryStream::Write<SymbolType>(fout, symbol);
    }
}
