#ifndef ALGORITHM_RLE_H_
#define ALGORITHM_RLE_H_ 1

#include <cstdint>
#include <istream>
#include <ostream>

namespace algorithm
{

class RLE
{
public:
    typedef uint8_t         SymbolType;
    typedef uint32_t        SizeType;       /** TODO: uint32_t is too big */
    typedef std::istream    StreamInType;
    typedef std::ostream    StreamOutType;

    void Encode(StreamInType & fin, StreamOutType & fout);
    void Decode(StreamInType & fin, StreamOutType & fout);
};

} /** ns: algorithm */

#endif /** ! ALGORITHM_RLE_H_ */
