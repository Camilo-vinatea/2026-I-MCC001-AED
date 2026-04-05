#include <cstddef>
#include "types.h"

// Los templates son una forma de escribir código genérico

template <typename T>
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

template <typename T>
Vector<T>::Vector() {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

template <typename T>
Vector<T>::~Vector() {
    delete[] m_data;
}

template <typename T>
void Vector<T>::push_back(T value) {
    if (m_size == m_capacity) {
        m_capacity = m_capacity == 0 ? 1 : m_capacity * 2;
        T *newData = new T[m_capacity];
        for (int i = 0; i < m_size; i++)
            newData[i] = m_data[i];
        delete[] m_data;
        m_data = newData;
    }
    m_data[m_size] = value;
    m_size++;
}

template <typename T>
T Vector<T>::get(size_t index) {
    return m_data[index];
}

template <typename T>
size_t Vector<T>::size() {
    return m_size;
}

void DemoVector();