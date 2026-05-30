#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <mutex>
#include <functional>
#include <utility>
#include "general_iterator.h"
#include "../types.h"
#include "../foreach.h"

using namespace std;

// Iterador hacia adelante — reutilizable por DoubleLinkedList
template <typename Container>
class LinkedListForwardIterator
    : public general_iterator<Container, LinkedListForwardIterator<Container>> {
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++() {
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Lista enlazada simple ordenada con nodo anidado dentro del contenedor
template <typename T, typename Comp = less<T>>
class LinkedList {
public:
    // Nodo anidado: define la estructura de dato y punteros de la lista
    struct Node {
        using value_type = T;
        T     m_data;
        Ref   m_ref;
        Node* m_pNext;

        Node() : m_pNext(nullptr) {}
        Node(T data, Ref ref, Node* pNext = nullptr)
            : m_data(data), m_ref(ref), m_pNext(pNext) {}
        virtual ~Node() = default;

        T      getData()    const { return m_data; }
        T&     getDataRef()       { return m_data; }
        void   setData(T d)       { m_data = d; }
        Ref    getRef()     const { return m_ref; }
        Ref&   getRefRef()        { return m_ref; }
        void   setRef(Ref r)      { m_ref = r; }
        Node*  getNext()    const { return m_pNext; }
        Node*& getNextRef()       { return m_pNext; }
        void   setNext(Node* p)   { m_pNext = p; }

        friend ostream& operator<<(ostream& os, const Node& n) {
            return os << "(" << n.m_data << ", " << n.m_ref << ")";
        }
    };

    using value_type       = T;
    using MySelf           = LinkedList<T, Comp>;
    using forward_iterator = LinkedListForwardIterator<MySelf>;

protected:
    Node*         m_pRoot = nullptr;
    Node*         m_pTail = nullptr;
    size_t        m_size  = 0;
    Comp          m_comp;
    mutable mutex m_mtx;

public:
    LinkedList() = default;

    // Constructor de copia
    LinkedList(const LinkedList& other) {
        Node* p = other.m_pRoot;
        while (p) { push_back(p->getData(), p->getRef()); p = p->getNext(); }
    }

    // Constructor de movimiento
    LinkedList(LinkedList&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size  = exchange(other.m_size,  size_t{0});
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            destruir_nodos();
            Node* p = other.m_pRoot;
            while (p) { push_back(p->getData(), p->getRef()); p = p->getNext(); }
        }
        return *this;
    }

    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            destruir_nodos();
            scoped_lock<mutex> lock(other.m_mtx);
            m_pRoot = exchange(other.m_pRoot, nullptr);
            m_pTail = exchange(other.m_pTail, nullptr);
            m_size  = exchange(other.m_size,  size_t{0});
        }
        return *this;
    }

    // Destructor seguro: libera todos los nodos en cadena
    virtual ~LinkedList() { destruir_nodos(); }

    virtual void push_front(value_type value, Ref ref) {
        Node* p = new Node(value, ref, m_pRoot);
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = p;
        if (!m_size) m_pTail = p;
        ++m_size;
    }

    virtual pair<value_type, Ref> pop_front() {
        scoped_lock<mutex> lock(m_mtx);
        if (!m_pRoot) throw out_of_range("pop_front(): lista vacía");
        Node* p  = m_pRoot;
        m_pRoot  = m_pRoot->getNext();
        if (!m_pRoot) m_pTail = nullptr;
        --m_size;
        auto r = make_pair(p->getData(), p->getRef());
        delete p;
        return r;
    }

    virtual void push_back(value_type value, Ref ref) {
        Node* p = new Node(value, ref);
        scoped_lock<mutex> lock(m_mtx);
        if (!m_size) { m_pRoot = p; m_pTail = p; }
        else { m_pTail->setNext(p); m_pTail = p; }
        ++m_size;
    }

    virtual pair<value_type, Ref> pop_back() {
        scoped_lock<mutex> lock(m_mtx);
        if (!m_pRoot) throw out_of_range("pop_back(): lista vacía");
        if (m_pRoot == m_pTail) {
            auto r = make_pair(m_pTail->getData(), m_pTail->getRef());
            delete m_pTail;
            m_pRoot = m_pTail = nullptr;
            --m_size;
            return r;
        }
        Node* p = m_pRoot;
        while (p->getNext() != m_pTail) p = p->getNext();
        auto r = make_pair(m_pTail->getData(), m_pTail->getRef());
        delete m_pTail;
        p->setNext(nullptr);
        m_pTail = p;
        --m_size;
        return r;
    }

    virtual void insert(const value_type& value, Ref ref) {
        insertar_interno(m_pRoot, value, ref);
    }

    virtual Node& operator[](size_t index) const {
        if (index >= m_size) throw out_of_range("índice fuera de rango");
        Node* p = m_pRoot;
        for (size_t i = 0; i < index; ++i) p = p->getNext();
        return *p;
    }

    virtual size_t size()  const { return m_size; }
    virtual bool   empty() const { return m_size == 0; }

    virtual string toString() {
        stringstream ss;
        Node* p = m_pRoot;
        ss << "[";
        if (m_size > 0) {
            for (size_t i = 0; i < m_size - 1; ++i) {
                ss << *p << ",";
                p = p->getNext();
            }
            ss << *p;
        }
        ss << "]";
        return ss.str();
    }

    forward_iterator begin() { return {this, m_pRoot}; }
    forward_iterator end()   { return {this, nullptr}; }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        unique_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args) {
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

private:
    void insertar_interno(Node*& pPrev, const value_type& value, Ref ref) {
        if (!pPrev || m_comp(value, pPrev->getDataRef())) {
            pPrev = new Node(value, ref, pPrev);
            ++m_size;
            if (pPrev == m_pRoot)
                m_pTail = pPrev;
            return;
        }
        insertar_interno(pPrev->getNextRef(), value, ref);
    }

    void destruir_nodos() {
        scoped_lock<mutex> lock(m_mtx);
        Node* p = m_pRoot;
        while (p) { Node* nx = p->getNext(); delete p; p = nx; }
        m_pRoot = m_pTail = nullptr;
        m_size = 0;
    }
};

template <typename T, typename Comp>
ostream& operator<<(ostream& os, LinkedList<T, Comp>& list) {
    return os << list.toString();
}

template <typename T, typename Comp>
istream& operator>>(istream& is, LinkedList<T, Comp>& list) {
    string line;
    getline(is, line);
    for (char& c : line)
        if (c == '[' || c == ']' || c == '(' || c == ')' || c == ',') c = ' ';
    T val; Ref ref;
    stringstream ss(line);
    while (ss >> val >> ref) list.push_back(val, ref);
    return is;
}

#endif // __LINKEDLIST_H__
