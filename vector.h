#include <cstddef>
#include "types.h"

class Vector{
    private:
        T *m_data;
        size_t  m_size;
        size_t m_capacity;
    public:
        Vector();
        ~Vector();
        void push_back(T value);
        T get(size_t index);
        size_t size();
};

void DemoVector();