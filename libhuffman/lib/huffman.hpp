#ifndef ALGORITHM_HUFFMAN_H_
#define ALGORITHM_HUFFMAN_H_ 1

#include <vector>
#include <functional>

namespace algorithm
{

class Huffman
{
public:
    typedef unsigned char   SymbolType;
    typedef size_t          SizeType;
    typedef unsigned int    CodewordType;

    typedef std::pair<SymbolType, SizeType> MetaSymbolType;

    struct Run
    {
        SymbolType      symbol;
        SizeType        run_len;
        SizeType        freq;

        Run *           left;
        Run *           right;

        CodewordType    codeword;
        SizeType        codeword_len;

        Run(const SymbolType & symbol, const SizeType & run_len, const SizeType & freq = 0)
        : symbol        (symbol)
        , run_len       (run_len)
        , freq          (freq)
        , left          (nullptr)
        , right         (nullptr)
        , codeword      (0)
        , codeword_len  (0)
        { }

        Run(const MetaSymbolType & meta_symbol, const SizeType & freq = 0)
        : symbol        (meta_symbol.first)
        , run_len       (meta_symbol.second)
        , freq          (freq)
        , left          (nullptr)
        , right         (nullptr)
        , codeword      (0)
        , codeword_len  (0)
        { }

        Run(const Run & run)
        : symbol        (run.symbol)
        , run_len       (run.run_len)
        , freq          (run.freq)
        , left          (run.left)
        , right         (run.right)
        , codeword      (run.codeword)
        , codeword_len  (run.codeword_len)
        { }

        Run(Run * left, Run * right)
        : symbol        (0)
        , run_len       (0)
        , freq          (left->freq + right->freq)
        , left          (left)
        , right         (right)
        , codeword      (0)
        , codeword_len  (0)
        { }

        inline
        Run &
        operator=(Run run)
        {
            this->symbol        = run.symbol;
            this->run_len       = run.run_len;
            this->freq          = run.freq;
            this->left          = run.left;
            this->right         = run.right;
            this->codeword      = run.codeword;
            this->codeword_len  = run.codeword_len;

            return *this;
        }

        inline
        bool
        operator==(const Run & rhs)
        const
        { return symbol == rhs.symbol && run_len == rhs.run_len; }

        inline
        bool
        operator!=(const Run & rhs)
        const
        { return ! (*this == rhs); }

        inline
        Run &
        operator++(void)
        {
            ++freq;
            return *this;
        }

        inline
        Run
        operator++(int)
        {
            Run temp(*this);
            operator++();
            return temp;
        }

        inline
        Run &
        operator--(void)
        {
            --freq;
            return *this;
        }

        inline
        Run
        operator--(int)
        {
            Run temp(*this);
            operator--();
            return temp;
        }

        inline
        bool
        operator<(const Run & rhs)
        const
        { return (this->freq < rhs.freq); }

        inline
        bool
        operator>(const Run & rhs)
        const
        { return (this->freq > rhs.freq); }

        inline
        bool
        operator<=(const Run & rhs)
        const
        { return ! operator>(rhs); }

        inline
        bool
        operator>=(const Run & rhs)
        const
        { return ! operator<(rhs); }
    };

    typedef std::vector<Run>    RunArrayType;
    typedef Run *               HuffmanTreeType;

private:
    RunArrayType        runs_;
    HuffmanTreeType     root_;

    void PrintHuffmanTree(FILE *, const Run *, const SizeType &);
    void AssignCodeword(Run *, const CodewordType &, const SizeType &);

public:
    void CollectRuns(std::ifstream &);
    void PrintAllRuns(FILE * = stdout);
    void CreateHuffmanTree(void);
    void PrintHuffmanTree(FILE * = stdout);
    void AssignCodeword(void);
};

} /** ns: algorithm */

#endif /** ! ALGORITHM_HUFFMAN_H_ */
