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
::CollectRuns(std::ifstream & f)
{
    typedef     std::map<MetaSymbolType, unsigned int>  CacheType;
    typedef     CacheType::iterator                     CacheIterType;

    char        raw;                /**<   signed char (std::ifstream::read() need signed char ptr) */
    SymbolType  symbol;             /**< unsigned char (ascii range is 0 to 255) */
    SymbolType  next_symbol;
    SizeType    run_len = 1;
    CacheType   cache;              /**< Caching a position of the run in the vector(runs_) */

    if(! f.eof())
    {
        f.read(&raw, sizeof(raw));
        symbol = (SymbolType)raw;   /** Cast signed to unsigned */

        while(! f.eof())
        {
            f.read(&raw, sizeof(raw));
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
                    runs_.push_back(Run(meta_symbol, 1));           /** First appreance; freq is 1 */
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
        CacheIterType   cache_iter = cache.find(meta_symbol);       /** Get the position from cache */
        Run             run(meta_symbol, 1);

        if(cache_iter == cache.end())
        {
            runs_.push_back(Run(meta_symbol, 1));                   /** First appreance; freq is 1 */
            cache.emplace(meta_symbol, runs_.size() - 1);           /** Cache the position */
        }
        else
            ++runs_.at(cache_iter->second);                         /** Add freq */
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
    Heap<Run>   heap;
    for(auto run : runs_)
        heap.Push(run);
    

    while(heap.size() > 1)
    {
        Run * left  = new Run(heap.Peek());
        heap.Pop();

        Run * right = new Run(heap.Peek());
        heap.Pop();

        Run temp(left, right);
        heap.Push(temp);
    }

    root_ = new Run(heap.Peek());
    heap.Pop();
}

void
Huffman
::PrintHuffmanTree(FILE * f)
{ PrintHuffmanTree(f, root_, 0); }

void
Huffman
::PrintHuffmanTree(FILE * f, const Run * node, const SizeType & depth)
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
::AssignCodeword(Run * node, const CodewordType & codeword, const SizeType & codeword_len) 
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
::AssignCodeword(void)
{ AssignCodeword(root_, 0, 0); }
