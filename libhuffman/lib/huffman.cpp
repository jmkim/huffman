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

using namespace algorithm;

void
Huffman
::CompressFile(FileInputType & fin, const StringType & fin_path, const StringType & fout_path)
{
    FileOutputType fout(fout_path, std::ios::binary);

    CollectRuns(fin);

    fin.clear();    /** Remove eofbit */
    WriteHeader(fin, fout);

    CreateHuffmanTree();
    AssignCodeword(root_, 0, 0);
    CreateRunList(root_);

    fin.seekg(0);   /** Reset the input position indicator */

    WriteEncode(fin, fout);

    fout.close();
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
::CollectRuns(FileInputType & fin)
{
    typedef     std::map<MetaSymbolType, unsigned int>  CacheType;
    typedef     CacheType::iterator                     CacheIterType;

    char        raw;                /**<   signed char (FileInputType::read() need signed char ptr) */
    SymbolType  symbol;             /**< unsigned char (ascii range is 0 to 255) */
    SymbolType  next_symbol;
    SizeType    run_len = 1;
    CacheType   cache;              /**< Caching a position of the run in the vector(runs_) */

    if(! fin.eof())
    {
        fin.read(&raw, sizeof(raw));
        symbol = (SymbolType)raw;   /** Cast signed to unsigned */

        while(! fin.eof())
        {
            fin.read(&raw, sizeof(raw));
            if(fin.eof()) break;

            next_symbol = (SymbolType)raw;

            if(symbol == next_symbol)
                ++run_len;
            else
            {
                /** Insert the pair into runs_;
                    key:    pair(symbol, run_len)
                    value:  appearance frequency of key
                */
                MetaSymbolType  meta_symbol = std::make_pair(symbol, run_len);
                CacheIterType   cache_iter = cache.find(meta_symbol);   /** Get the position from cache */

                if(cache_iter == cache.end())
                {
                    runs_.push_back(RunType(meta_symbol, 1));       /** First appreance; freq is 1 */
                    cache.emplace(meta_symbol, runs_.size() - 1);   /** Cache the position */
                }
                else
                    ++runs_.at(cache_iter->second);                 /** Add freq */

                run_len = 1;
            }

            symbol = next_symbol;
        }

        /** Process the remaining symbol */
        MetaSymbolType  meta_symbol = std::make_pair(symbol, run_len);
        CacheIterType   cache_iter = cache.find(meta_symbol);   /** Get the position from cache */

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
int count;

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

bool
Huffman
::GetCodeword(CodewordType & codeword, const SymbolType & symbol, const SizeType & run_len)
{
    RunType * n = list_.at(symbol);
    for(; n != nullptr && n->run_len != run_len; n = n->next);

    if(n == nullptr)
        return false;
    else
    {
        codeword = n->codeword;
        return true;
    }
}

void
Huffman
::WriteHeader(FileInputType & fin, FileOutputType & fout)
{
    WriteToFile<RunArrayType::size_type>    (fout, runs_.size() );
    WriteToFile<FileInputType::pos_type>    (fout, fin.tellg()  );

    for(auto run : runs_)
    {
        WriteToFile<SymbolType> (fout, run.symbol   );
        WriteToFile<SizeType>   (fout, run.run_len  );
        WriteToFile<SizeType>   (fout, run.freq     );
    }
}

void
Huffman
::WriteEncode(FileInputType & fin, FileOutputType & fout)
{
    char        raw;                /**<   signed char (FileInputType::read() need signed char ptr) */
    SymbolType  symbol;             /**< unsigned char (ascii range is 0 to 255) */
    SymbolType  next_symbol;
    SizeType    run_len = 1;

    EncodeBufferType    buffer      = 0; /**< Buffer */
    EncodeBufferType    buffer_stat = std::numeric_limits<EncodeBufferType>::max();

    if(! fin.eof())
    {
        fin.read(&raw, sizeof(raw));
        symbol = (SymbolType)raw;   /** Cast signed to unsigned */

        while(! fin.eof())
        {
            fin.read(&raw, sizeof(raw));
            if(fin.eof()) break;

            next_symbol = (SymbolType)raw;

            if(symbol == next_symbol)
                ++run_len;
            else
            {
                /** Insert the pair into runs_;
                    key:    pair(symbol, run_len)
                    value:  appearance frequency of key
                */
                CodewordType codeword;
                bool cw_found = GetCodeword(codeword, symbol, run_len);

                if(cw_found == false)
                    return; /* TODO: Exception(Codeword not found)  */

                while(codeword != 0)
                {
                    EncodeBufferType temp = codeword % 2;

                    codeword    = codeword >> 1;

                    buffer      = buffer << 1;
                    buffer_stat = buffer_stat >> 1;

                    buffer += temp;

                    if(buffer_stat == 0)
                    {
                        WriteToFile<EncodeBufferType> (fout, buffer);

                        buffer      = 0;
                        buffer_stat = std::numeric_limits<EncodeBufferType>::max();
                    }
                }

                run_len = 1;
            }

            symbol = next_symbol;
        }

        /** Process the remaining symbol */
        CodewordType codeword;
        bool cw_found = GetCodeword(codeword, symbol, run_len);

        if(cw_found == false)
            return; /* TODO: Exception(Codeword not found)  */

        while(codeword != 0)
        {
            EncodeBufferType temp = codeword % 2;
            codeword = codeword >> 1;
            buffer = buffer << 1;
            buffer += temp;

            if(buffer > std::numeric_limits<EncodeBufferType>::max())
                WriteToFile<EncodeBufferType> (fout, buffer);
        }

        run_len = 1;
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
