#ifndef ALGORITHM_HUFFMAN_H_
#define ALGORITHM_HUFFMAN_H_ 1

#include <fstream>
#include <vector>
#include <array>
#include <limits>
#include <string>
#include <cstring>

namespace algorithm
{

/** \brief  Modified Huffman coding

    Huffman coding method with Run Length Encoding.
    Combines the variable length codes of Huffman coding with the
    coding of repetitive data in Run Length Encoding.
*/
class Huffman
{
public:
    typedef size_t          SizeType;

    typedef std::ifstream   StreamInType;
    typedef std::ofstream   StreamOutType;

    typedef std::string     StringType;

    typedef uint8_t         ByteType;       /** One byte */
    typedef uint32_t        CodewordType;   /** Codeword, consists of 4 bytes */

    typedef std::pair<ByteType, SizeType> MetaSymbolType;

    /** Constants */
    static  const ByteType  zerobit     = 0;
    static  const ByteType  nonzerobit  = 1;
    static  const SizeType  ascii_max   = std::numeric_limits<ByteType>::max();

    static  const SizeType  byte_size   = 8;                /**< 8 bits, bit size of ByteType */
    static  const SizeType  buffer_size = byte_size * 4;    /**< Bit size of CodewordType */

    /** Class for each node in Huffman tree, and used in RLE */
    struct Run
    {
        /** MetaSymbol, paired with MetaSymbolType */
        ByteType        symbol;         /**< ASCII character */
        SizeType        run_len;        /**< Run-length, used in RLE */

        /** For Heap, and Huffman coding */
        SizeType        freq;           /**< Frequency of the MetaSymbol */

        /** For Huffman Tree */
        Run *           left;           /**< Left child node */
        Run *           right;          /**< Right child node */
        CodewordType    codeword;       /**< Huffman codeword */
        SizeType        codeword_len;   /**< Length of member `codeword' */

        /** For ArrayList */
        Run *           next;

        /** Contructors */
        Run(const ByteType & symbol, const SizeType & run_len, const SizeType & freq = 0)
        : symbol        (symbol)
        , run_len       (run_len)
        , freq          (freq)
        , left          (nullptr)
        , right         (nullptr)
        , codeword      (0)
        , codeword_len  (0)
        , next          (nullptr)
        { }

        Run(const MetaSymbolType & meta_symbol, const SizeType & freq = 0)
        : symbol        (meta_symbol.first)
        , run_len       (meta_symbol.second)
        , freq          (freq)
        , left          (nullptr)
        , right         (nullptr)
        , codeword      (0)
        , codeword_len  (0)
        , next          (nullptr)
        { }

        Run(const Run & run)
        : symbol        (run.symbol)
        , run_len       (run.run_len)
        , freq          (run.freq)
        , left          (run.left)
        , right         (run.right)
        , codeword      (run.codeword)
        , codeword_len  (run.codeword_len)
        , next          (run.next)
        { }

        Run(Run * left, Run * right)
        : symbol        (0)
        , run_len       (0)
        , freq          (left->freq + right->freq)
        , left          (left)
        , right         (right)
        , codeword      (0)
        , codeword_len  (0)
        , next          (nullptr)
        { }

        /** Operators */
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
            this->next          = run.next;

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

    typedef Run     RunType;

    typedef std::vector<RunType>    RunArrayType;
    typedef RunType *               HuffmanTreeType;
    typedef std::array<RunType *, ascii_max + 1>    RunListType;

private:
    /** Member data */
    RunArrayType        runs_;      /** Set of runs */
    RunListType         list_;      /** ArrayList of runs, for symbol searching efficiency */
    HuffmanTreeType     root_;      /** Root node of the Huffman tree */

    /** Member functions */
    void CollectRuns(StreamInType &);
    void CreateHuffmanTree(void);
    void AssignCodeword(RunType *, const CodewordType & = 0, const SizeType & = 0);
    void CreateRunList(RunType *);
    SizeType GetCodeword(CodewordType &, const ByteType &, const SizeType &);
    void WriteHeader(StreamInType &, StreamOutType &);
    void WriteEncode(StreamInType &, StreamOutType &);
    void WriteDecode(StreamInType &, StreamOutType &);
    void PrintHuffmanTree(FILE *, const RunType *, const SizeType &);
    SizeType ReadHeader(StreamInType &);

public:
    void CompressFile(StreamInType &, const StringType &, const StringType &);
    void DecompressFile(StreamInType &, const StringType &, const StringType &);
    void PrintAllRuns(FILE * = stdout);

    inline
    void
    PrintHuffmanTree(FILE * fout = stdout)
    { PrintHuffmanTree(fout, root_, 0); }
};

} /** ns: algorithm */

#endif /** ! ALGORITHM_HUFFMAN_H_ */
