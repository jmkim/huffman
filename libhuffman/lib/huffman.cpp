#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include "huffman.hpp"
#include "heap.hpp"

using namespace algorithm;

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
::PrintAllRuns(FILE * f)
{
    fprintf(f, "SYM LENG FREQ\n");  /** Header of data */

    for(auto run : runs_)
        fprintf(f, " %02x %4d %d\n", run.symbol, run.run_len, run.freq);
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
::PrintHuffmanTree(FILE * f, const RunType * node, const SizeType & depth)
{
    for(int i = 0; i < depth; ++i)
        fprintf(f, " ");
    if(node == nullptr)
        fprintf(f, "null\n");
    else
    {
        fprintf(f, "%02x:%d:%d:%d %d\n"
                    , node->symbol
                    , node->run_len
                    , node->freq
                    , node->codeword 
                    , node->codeword_len);

        PrintHuffmanTree(f, node->left,  depth + 1);
        PrintHuffmanTree(f, node->right, depth + 1);
    }
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
        CreateRunList(node->left);
        CreateRunList(node->right);
    }
}

Huffman
::RunType *
Huffman
::GetRunFromList(const SymbolType & symbol, const SizeType & run_len)
{
    if(list_.at(symbol) == nullptr || list_.at(symbol)->run_len == run_len)
        return list_.at(symbol);
    else
    {
        RunType *n;
        for(n = list_.at(symbol); n->run_len == run_len || n->next != nullptr; n = n->next);

        return n->next;
    }
}

void
Huffman
::CompressFile(FileInputType & fin, const StringType & fin_path, const StringType & fout_path);
{
    FileOutputType fout(fout_path, std::ios::binary);

    CollectRuns(fin);
    WriteHeader(fin, fout);
    CreateHuffmanTree();
    AssignCodeword(root_, 0, 0);
    CreateRunList(root_);

    fin.seekg(0); /** Reset the input position indicator */

    WriteEncode(fin, fout);
}

void
Huffman
::WriteHeader(FileInputType & fin, FileOutputType & fout)
{
    fout.write(reinperpret_cast<char *>(&runs_.size(), sizeof(runs_.size()));   /** Count of runs */
    fout.write(reinperpret_cast<char *>(&fin.tellg()), sizeof(fin.tellg()));    /** Size of the input file */

    for(auto run : runs_)
    {
        fout.write(reinperpret_cast<char *>(&run.symbol),  sizeof(run.symbol));
        fout.write(reinperpret_cast<char *>(&run.run_len), sizeof(run.run_len));
        fout.write(reinperpret_cast<char *>(&run.freq),    sizeof(run.freq));
    }
}

void
Huffman
::WriteEncode(FileInputType & fin, FileOutputType & fout)
{
    while(! fin.eof())
    {
        EncodeBufferType buffer = 0;
    }

}

void
Huffman
::RecogniseRun(FileInputType & fin)
{
}
