#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cstddef>
#include <sstream>
#include "types.h"

// Los templates son una forma de escribir código genérico

template <typename T>
struct VectorNode{
    T   m_data;
    Ref m_ref;
    VectorNode() : m_data(T()), m_ref(Ref()) {}
    VectorNode(T data, Ref ref) : m_data(data), m_ref(ref) {}
    string ToString(){
        ostringstream oss;
        oss << "(" << m_data << "," << m_ref << ")";
        return oss.str();
    }
    T   GetData() const { return m_data; }
    Ref GetRef()  const { return m_ref;  }
};

template <typename T>
ostream& operator<<(ostream& os, VectorNode<T>& vn){
    return os << vn.ToString();
}

template <typename T>
class Vector{
    using Node = VectorNode<T>;
    private:
        Node * m_data;
        size_t m_size;
        size_t m_capacity;
    private:
        void resize();
    public:
        Vector(size_t capacity = 10);
        ~Vector();
        void push_back(T value, Ref ref);
        T get(size_t index);
        size_t size();
        string ToString();
};

template <typename T>
Vector<T>::Vector(size_t capacity) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = capacity;
    m_data = new Node[m_capacity];
}

template <typename T>
Vector<T>::~Vector() {
    delete[] m_data;
}

template <typename T>
void Vector<T>::push_back(T value, Ref ref) {
    if (m_size == m_capacity)
        resize();
    m_data[m_size] = Node(value, ref);
    m_size++;
}

template <typename T>
void Vector<T>::resize() {
    m_capacity = m_capacity < 10 ? 10: m_capacity * 2;
    Node *newData = new Node[m_capacity];
    for (size_t i = 0; i < m_size; i++)
        newData[i] = m_data[i];
    delete[] m_data;
    m_data = newData;
}

template <typename T>
T Vector<T>::get(size_t index) {
    if (index >= m_size)
        throw out_of_range("Index out of bounds");
    return m_data[index].GetData();
}

template <typename T>
size_t Vector<T>::size() {
    return m_size;
}

// [1, 2, 3, 4, 5]
template <typename T>
string Vector<T>::ToString() {
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < m_size-1; i++)
        oss << m_data[i] << ",";
    if (m_size > 0)
        oss << m_data[m_size-1];
    oss << "]";
    return oss.str();
}

template <typename T>
ostream& operator<<(ostream& os, Vector<T>& v){
    return os << v.ToString();
}

void DemoVector();

#endif // __VECTOR_H__