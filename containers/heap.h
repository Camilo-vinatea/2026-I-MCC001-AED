#ifndef __HEAP_H__
#define __HEAP_H__

#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <stdexcept>
#include <utility>
#include "basetrait.h"
#include "../types.h"

using namespace std;

/*
Traits

Reutilizan AscendingTrait / DescendingTrait de basetrait.h para Comp.
AscendingHeapTrait : min-heap — raíz es mínimo, extrae en orden ascendente.
DescendingHeapTrait: max-heap — raíz es máximo, extrae en orden descendente.
Node no forma parte del trait — está definido dentro de Heap.
*/
template <typename T>
struct BaseHeapTrait {
    using value_type = T;
};

template <typename T>
struct AscendingHeapTrait : public BaseHeapTrait<T>,
                            public AscendingTrait<T> {};

template <typename T>
struct DescendingHeapTrait : public BaseHeapTrait<T>,
                             public DescendingTrait<T> {};


template <typename Traits>
class Heap {
public:
    using value_type = typename Traits::value_type;
    using Comp       = typename Traits::Comp;
    using MySelf     = Heap<Traits>;

    struct Node {
        value_type m_data;

        Node() = default;
        Node(value_type data) : m_data(data) {}

        value_type  getData()    const { return m_data; }
        value_type& getDataRef()       { return m_data; }
        void        setData(value_type d) { m_data = d; }

        friend ostream& operator<<(ostream& os, const Node& n) {
            return os << n.m_data;
        }
    };

private:
    static constexpr size_t DEFAULT_CAPACITY = 10;

    Node*         m_data;
    size_t        m_size;
    size_t        m_capacity;
    Comp          m_comp;
    mutable mutex m_mtx;

    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t left  (size_t i) const { return 2 * i + 1;   }
    size_t right (size_t i) const { return 2 * i + 2;   }

    void heapify_up(size_t i) {
        while (i > 0 && m_comp(m_data[i].m_data, m_data[parent(i)].m_data)) {
            swap(m_data[i], m_data[parent(i)]);
            i = parent(i);
        }
    }

    void heapify_down(size_t i) {
        size_t target = i;
        size_t l = left(i);
        size_t r = right(i);
        if (l < m_size && m_comp(m_data[l].m_data, m_data[target].m_data)) target = l;
        if (r < m_size && m_comp(m_data[r].m_data, m_data[target].m_data)) target = r;
        if (target != i) {
            swap(m_data[i], m_data[target]);
            heapify_down(target);
        }
    }

    // Llamado siempre bajo lock — no adquiere lock propio.
    void resize() {
        size_t newCap  = m_capacity * 2;
        Node*  newData = new Node[newCap];
        for (size_t i = 0; i < m_size; ++i)
            newData[i] = move(m_data[i]);
        delete[] m_data;
        m_data     = newData;
        m_capacity = newCap;
    }

public:

    explicit Heap(size_t capacity = DEFAULT_CAPACITY)
        : m_data(new Node[capacity]), m_size(0), m_capacity(capacity) {}

    // COPY CONSTRUCTOR
    Heap(const Heap& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        m_data     = new Node[other.m_capacity];
        m_size     = other.m_size;
        m_capacity = other.m_capacity;
        for (size_t i = 0; i < m_size; ++i)
            m_data[i] = other.m_data[i];
    }

    // MOVE CONSTRUCTOR
    Heap(Heap&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        m_data     = exchange(other.m_data,     nullptr);
        m_size     = exchange(other.m_size,     0);
        m_capacity = exchange(other.m_capacity, 0);
    }


    // DESTRUCTOR
    ~Heap() {
        scoped_lock<mutex> lock(m_mtx);
        delete[] m_data;
        m_data     = nullptr;
        m_size     = 0;
        m_capacity = 0;
    }



    void insert(const value_type& value) {
        scoped_lock<mutex> lock(m_mtx);
        if (m_size == m_capacity) resize();
        m_data[m_size] = Node(value);
        heapify_up(m_size);
        ++m_size;
    }

    Node extract() {
        scoped_lock<mutex> lock(m_mtx);
        if (m_size == 0)
            throw out_of_range("extract(): empty heap");
        Node root = m_data[0];
        --m_size;
        if (m_size > 0) {
            m_data[0] = move(m_data[m_size]);
            heapify_down(0);
        }
        return root;
    }

    const Node& peek() const {
        scoped_lock<mutex> lock(m_mtx);
        if (m_size == 0)
            throw out_of_range("peek(): empty heap");
        return m_data[0];
    }

    size_t size()  const { return m_size;      }
    bool   empty() const { return m_size == 0; }

    string toString() const {
        scoped_lock<mutex> lock(m_mtx);
        ostringstream ss;
        ss << "[";
        for (size_t i = 0; i < m_size; ++i) {
            ss << m_data[i];
            if (i < m_size - 1) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        unique_lock<mutex> lock(m_mtx);
        for (size_t i = 0; i < m_size; ++i)
            func(m_data[i], forward<Args>(args)...);
    }

    /*
    Mejora #1 — build (Floyd's algorithm)

    Construye el heap a partir de un arreglo en O(n).
    Más eficiente que n inserciones individuales: O(n) vs O(n log n).
    Aplica heapify_down desde el último nodo interno hacia la raíz.
    */
    void build(const value_type* values, size_t n) {
        scoped_lock<mutex> lock(m_mtx);
        if (n > m_capacity) {
            delete[] m_data;
            m_capacity = n * 2;
            m_data     = new Node[m_capacity];
        }
        m_size = n;
        for (size_t i = 0; i < n; ++i)
            m_data[i] = Node(values[i]);
        if (n > 1)
            for (int i = (int)(n / 2) - 1; i >= 0; --i)
                heapify_down((size_t)i);
    }

    /*
    Mejora #2 — replace

    Reemplaza la raíz con un nuevo elemento y reordena en O(log n).
    Equivalente a extract() + insert() pero sin heapify_up — un solo
    recorrido descendente.
    */
    Node replace(const value_type& value) {
        scoped_lock<mutex> lock(m_mtx);
        if (m_size == 0)
            throw out_of_range("replace(): empty heap");
        Node old   = m_data[0];
        m_data[0]  = Node(value);
        heapify_down(0);
        return old;
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, const Heap<Traits>& heap) {
    return os << heap.toString();
}

template <typename Traits>
istream& operator>>(istream& is, Heap<Traits>& heap) {
    using value_type = typename Heap<Traits>::value_type;
    string line;
    getline(is, line);
    for (char& c : line)
        if (c == '[' || c == ']' || c == ',')
            c = ' ';
    value_type value;
    stringstream ss(line);
    while (ss >> value)
        heap.insert(value);
    return is;
}

#endif // __HEAP_H__