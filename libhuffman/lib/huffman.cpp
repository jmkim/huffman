#include "huffman.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <cstring>
#include <limits>

#include "heap.hpp"
#include "binarystream.hpp"

using namespace algorithm;

void
Huffman
::CompressFile(StreamInType & fin, StreamOutType & fout)
{
    CollectRuns(fin);

    PrintAllRuns();

    fin.clear();            /** Remove eofbit */
    WriteHeader(fin, fout);

    CreateHuffmanTree();
    AssignCodeword(root_, 0, 0);

    PrintHuffmanTree();

    CreateRunList(root_);
    WriteEncode(fin, fout);
}

void
Huffman
::DecompressFile(StreamInType & fin, StreamOutType & fout)
{
    ReadHeader(fin);

    PrintAllRuns();

    CreateHuffmanTree();
    AssignCodeword(root_, 0, 0);

    PrintHuffmanTree();

    WriteDecode(fin, fout);
}

void
Huffman
::PrintAllRuns(FILE * f)
{
    fprintf(f, "SYM LENG FREQ\n");  /** Header of data */

    for(auto run : runs_)
        fprintf(f, " %02x %4lu %lu\n", run.symbol, run.run_len, run.freq);
}

void
Huffman
::CollectRuns(StreamInType & fin)
{
    typedef     std::map<MetaSymbolType, unsigned int>  CacheType;
    typedef     CacheType::iterator                     CacheIterType;

    CacheType   cache;              /**< Caching a position of the run in the vector(runs_) */

    fin.clear();                    /** Reset the input position indicator */
    fin.seekg(0, fin.beg);

    while(! fin.eof())
    {
        ByteType    symbol;
        SizeType    run_len = 1;

        BinaryStream::Read<ByteType>(fin, symbol);
        if(fin.eof())
            break;

        //BinaryStream::Read<SizeType>(fin, run_len);

        /** Insert the pair into runs_;
            key:    pair(symbol, run_len)
            value:  appearance frequency of key
        */
        MetaSymbolType  meta_symbol = std::make_pair(symbol, run_len);
        CacheIterType   cache_iter  = cache.find(meta_symbol);  /** Get the position from cache */

        if(cache_iter == cache.end())
        {
            runs_.push_back(RunType(meta_symbol, 1));           /** First appreance; freq is 1 */
            cache.emplace(meta_symbol, runs_.size() - 1);       /** Cache the position */
        }
        else
            ++runs_.at(cache_iter->second);                     /** Add freq */
    }
}

void
Huffman
::CreateHuffmanTree(void)
{
    Heap<RunType>   heap;
    for(auto run : runs_)
        heap.Push(run);
    

    while(heap.size() > 1)
    {
        RunType * left  = new RunType(heap.Peek());
        heap.Pop();

        RunType * right = new RunType(heap.Peek());
        heap.Pop();

        RunType temp(left, right);
        heap.Push(temp);
    }

    root_ = new RunType(heap.Peek());
    heap.Pop();
}

void
Huffman
::AssignCodeword(RunType * node, const CodewordType & codeword, const SizeType & codeword_len)
{
    if(node->left == nullptr && node->right == nullptr)
    {
        node->codeword      = codeword;
        node->codeword_len  = codeword_len;
    }
    else
    {
        AssignCodeword(node->left,  (codeword << 1) + 0, codeword_len + 1);
        AssignCodeword(node->right, (codeword << 1) + 1, codeword_len + 1);
    }
}

void
Huffman
::CreateRunList(RunType * node)
{
    if(node->left == nullptr && node->right == nullptr)
    {
        //printf("[%4d] %02x:%d:%d:%d %x\n", ++count, node->symbol, node->run_len, node->freq, node->codeword_len, node->codeword);
        if(list_.at(node->symbol) == nullptr)
            list_.at(node->symbol) = node;
        else
        {
            node->next = list_.at(node->symbol);
            list_.at(node->symbol) = node;
        }
    }
    else
    {
        if(node->left != nullptr)
            CreateRunList(node->left);

        if(node->right != nullptr)
            CreateRunList(node->right);
    }
}

Huffman
::SizeType
Huffman
::GetCodeword(CodewordType & codeword, const ByteType & symbol, const SizeType & run_len)
{
    RunType * n = list_.at(symbol);
    for(; n != nullptr && n->run_len != run_len; n = n->next);

    if(n == nullptr)
        return 0;
    else
    {
        codeword = n->codeword;
        return n->codeword_len;
    }
}

void
Huffman
::WriteHeader(StreamInType & fin, StreamOutType & fout)
{
    uint16_t run_size = uint16_t(runs_.size());
    BinaryStream::Write<uint16_t>(fout, run_size);

    fin.clear();
    fin.seekg(0, fin.end);

    uint32_t fin_size = uint32_t(fin.tellg());
    BinaryStream::Write<uint32_t>(fout, SizeType(fin.tellg()));

    for(auto run : runs_)
    {
        BinaryStream::Write<ByteType>(fout, run.symbol);
        BinaryStream::Write<SizeType>(fout, run.run_len);
        BinaryStream::Write<SizeType>(fout, run.freq);
    }
}

void
Huffman
::WriteEncode(StreamInType & fin, StreamOutType & fout)
{
    const   SizeType            bufstat_max     = buffer_size;
            SizeType            bufstat_free    = bufstat_max;
            CodewordType        buffer          = 0;

    /** Reset the input position indicator */
    fin.clear();
    fin.seekg(0, fin.beg);

    while(! fin.eof())
    {
        ByteType    symbol;
        SizeType    run_len = 1;

        BinaryStream::Read<ByteType>(fin, symbol);
        if(fin.eof())
            break;

        /** Write the codeword to fout */

        CodewordType    codeword;
        SizeType        codeword_len = GetCodeword(codeword, symbol, run_len);

        if(codeword_len == 0)
            return; /* TODO: Exception(Codeword not found)  */

        while(codeword_len >= bufstat_free)
        {
            buffer <<= bufstat_free;
            buffer += (codeword >> (codeword_len - bufstat_free));
            codeword = codeword % (0x1 << codeword_len - bufstat_free);
            codeword_len -= bufstat_free;

            BinaryStream::Write<CodewordType>(fout, buffer, false);

            buffer = 0;
            bufstat_free = bufstat_max;
        }

        buffer <<= codeword_len;
        buffer += codeword;
        bufstat_free -= codeword_len;
    }
}

void
Huffman
::PrintHuffmanTree(FILE * f, const RunType * node, const SizeType & depth)
{
    for(int i = 0; i < depth; ++i)
        fprintf(f, "  ");
    if(node == nullptr)
        fprintf(f, "null\n");
    else
    {
        fprintf(f, "%02x:%lu:%lu:%lu %x\n"
                    , node->symbol
                    , node->run_len
                    , node->freq
                    , node->codeword_len
                    , node->codeword);

        PrintHuffmanTree(f, node->left,  depth + 1);
        PrintHuffmanTree(f, node->right, depth + 1);
    }
}

Huffman::
SizeType
Huffman::
ReadHeader(StreamInType & fin)
{
    typedef     std::map<MetaSymbolType, unsigned int>  CacheType;
    typedef     CacheType::iterator                     CacheIterType;

    fin.clear();
    fin.seekg(0, fin.beg);

    uint16_t run_size;
    BinaryStream::Read<uint16_t>(fin, run_size);

    uint32_t fin_size;
    BinaryStream::Read<uint32_t>(fin, fin_size);

    runs_.clear();

    for(int i = 0; i < run_size; ++i)
    {
        ByteType symbol;
        BinaryStream::Read<ByteType>(fin, symbol);

        SizeType run_len;
        BinaryStream::Read<SizeType>(fin, run_len);

        SizeType freq;
        BinaryStream::Read<SizeType>(fin, freq);

        Run temp = Run(symbol, run_len, freq);
        runs_.push_back(temp);
    }
}

void
Huffman::
WriteDecode(StreamInType & fin, StreamOutType & fout)
{
    const   SizeType            bufstat_max     = buffer_size;
            SizeType            bufstat_free    = bufstat_max;
            CodewordType        buffer          = 0;
            Run *               run             = root_;

    fout.clear();
    fout.seekp(0, fin.beg);

    while(! fin.eof())
    {
        BinaryStream::Read<CodewordType>(fin, buffer);
        bufstat_free = 0;

        while(bufstat_free < bufstat_max)
        {
            if(buffer / (0x1 << (buffer_size - 1)) == zerobit)
                run = run->left;
            else
                run = run->right;

            buffer <<= 0x1;
            ++bufstat_free;

            if(run->codeword_len > 0)
            {
                for(int i = 0; i < run->run_len; ++i)
                    BinaryStream::Write<ByteType>(fout, run->symbol);

                run = root_;
            }
        }
    }
}
