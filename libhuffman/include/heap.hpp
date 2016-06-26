#ifndef ALGORITHM_HEAP_H_
#define ALGORITHM_HEAP_H_ 1

#include <functional>
#include <vector>

namespace algorithm
{

template<
    typename ElementType,
    typename ElementCompareFunc = std::less<ElementType>,
    typename HeapType           = std::vector<ElementType>
>
class Heap : public HeapType
{
private:
    void
    Heapify(const int pos, const int depth)
    {
        if(depth == 0)
            return;

        if(2 * pos + 1 > this->size() - 1)
            return;
        else if(ElementCompareFunc()(this->at(2 * pos + 1), this->at(pos)))
        {
            std::swap(this->at(2 * pos + 1), this->at(pos));
            Heapify(2 * pos + 1, depth - 1);
            Heapify(pos, depth);
        }
        else if(2 * pos + 2 > this->size() - 1)
            return;
        else if(ElementCompareFunc()(this->at(2 * pos + 2), this->at(pos)))
        {
            std::swap(this->at(2 * pos + 2), this->at(pos));
            Heapify(2 * pos + 2, depth - 1);
            Heapify(pos, depth);
        }
    }

public:
    void
    Push(const ElementType & element)
    {
        this->push_back(element);

        for(int i = this->size() - 1; i > 0; i = (i - 1) / 2)
        {
            if(ElementCompareFunc()(this->at(i), this->at((i - 1) / 2)))
                std::swap(this->at(i), this->at((i - 1) / 2));
        }
    }

    void
    Pop(void)
    {
        std::swap(this->at(0), this->at(this->size() - 1));
        this->pop_back();

        int depth = 0;
        for(int i = 0, j = 0; j < this->size(); j += ((i == 0) ? ++i : (i *= 2)))
            ++depth;

        Heapify(0, depth);
    }

    ElementType
    Peek(void)
    const
    { return this->front(); }
};

}

#endif /** ! ALGORITHM_HEAP_H_ */
