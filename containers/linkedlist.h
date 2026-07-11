//! @file linkedlist.h
//! @brief Lista enlazada simple ordenada con nodo anidado y concurrencia.
//! @details Implementa `LinkedList<T, Comp>` y un iterador forward
//!          reutilizable también por `DoubleLinkedList`.
//! @author Equipo MCC

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

//! @class LinkedListForwardIterator
//! @brief Iterador forward que avanza siguiendo `m_pNext`.
template <typename Container>
class LinkedListForwardIterator
    : public general_iterator<Container, LinkedListForwardIterator<Container>>
{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    //! @brief Avanza al siguiente nodo.
    MySelf& operator++() {
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

//! @class LinkedList
//! @brief Lista enlazada simple ordenada con nodo anidado.
//! @tparam T    Tipo de dato almacenado.
//! @tparam Comp Comparador; por defecto `less<T>` (orden ascendente).
template <typename T, typename Comp = less<T>>
class LinkedList {
public:
    //! @struct Node
    //! @brief Nodo con dato, `Ref` y puntero al siguiente.
    struct Node {
        using value_type = T;
        T     m_data;     //!< Valor almacenado.
        Ref   m_ref;      //!< Identificador asociado.
        Node* m_pNext;    //!< Siguiente nodo.

        //! @brief Constructor por defecto (útil para `vector` o `resize`).
        Node() : m_pNext(nullptr) {}
        //! @brief Constructor con dato, `Ref` y puntero opcional al siguiente.
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

        //! @brief Imprime el nodo como `(data, ref)`.
        friend ostream& operator<<(ostream& os, const Node& n) {
            return os << "(" << n.m_data << ", " << n.m_ref << ")";
        }
    };

    using value_type       = T;
    using MySelf           = LinkedList<T, Comp>;
    using forward_iterator = LinkedListForwardIterator<MySelf>;

protected:
    Node*         m_pRoot = nullptr;  //!< Primer nodo.
    Node*         m_pTail = nullptr;  //!< Último nodo.
    size_t        m_size  = 0;        //!< Número de nodos.
    Comp          m_comp;             //!< Comparador de orden.
    mutable mutex m_mtx;              //!< Mutex para concurrencia.

public:
    //! @brief Constructor por defecto.
    LinkedList() = default;

    //! @brief Constructor de copia: copia todos los elementos.
    LinkedList(const LinkedList& other) {
        Node* p = other.m_pRoot;
        while (p) { push_back(p->getData(), p->getRef()); p = p->getNext(); }
    }

    //! @brief Constructor de movimiento. Toma el control del otro contenedor.
    LinkedList(LinkedList&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_pTail = exchange(other.m_pTail, nullptr);
        m_size  = exchange(other.m_size,  size_t{0});
    }

    //! @brief Operador de asignación por copia.
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            destruir_nodos();
            Node* p = other.m_pRoot;
            while (p) { push_back(p->getData(), p->getRef()); p = p->getNext(); }
        }
        return *this;
    }

    //! @brief Operador de asignación por movimiento.
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

    //! @brief Destructor: libera todos los nodos en cadena.
    virtual ~LinkedList() { destruir_nodos(); }

    //! @brief Inserta un nodo al frente de la lista.
    //! @param value Valor a insertar.
    //! @param ref   Referencia asociada.
    virtual void push_front(value_type value, Ref ref) {
        Node* p = new Node(value, ref, m_pRoot);
        scoped_lock<mutex> lock(m_mtx);
        m_pRoot = p;
        if (!m_size) m_pTail = p;
        ++m_size;
    }

    //! @brief Saca el primer nodo y devuelve `(valor, ref)`.
    //! @throw std::out_of_range Si la lista está vacía.
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

    //! @brief Inserta un nodo al final de la lista.
    virtual void push_back(value_type value, Ref ref) {
        Node* p = new Node(value, ref);
        scoped_lock<mutex> lock(m_mtx);
        if (!m_size) { m_pRoot = p; m_pTail = p; }
        else { m_pTail->setNext(p); m_pTail = p; }
        ++m_size;
    }

    //! @brief Saca el último nodo y devuelve `(valor, ref)`.
    //! @throw std::out_of_range Si la lista está vacía.
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

    //! @brief Inserción ordenada en la posición correcta según `Comp`.
    virtual void insert(const value_type& value, Ref ref) {
        insertar_interno(m_pRoot, value, ref);
    }

    //! @brief Acceso aleatorio O(n) por índice (sobrecarga `operator[]`).
    //! @throw std::out_of_range Si el índice está fuera de rango.
    virtual Node& operator[](size_t index) const {
        if (index >= m_size) throw out_of_range("índice fuera de rango");
        Node* p = m_pRoot;
        for (size_t i = 0; i < index; ++i) p = p->getNext();
        return *p;
    }

    //! @brief Número de nodos en la lista.
    virtual size_t size()  const { return m_size; }
    //! @brief `true` si la lista está vacía.
    virtual bool   empty() const { return m_size == 0; }

    //! @brief Representación textual estilo `(1,2),(2,3),...`.
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

    //! @brief Iterador al primer nodo.
    forward_iterator begin() { return {this, m_pRoot}; }
    //! @brief Iterador "uno más allá" del último.
    forward_iterator end()   { return {this, nullptr}; }

    //! @brief Recorre la lista aplicando `func(elem, args...)`.
    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        unique_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    //! @brief Primer nodo cuyo predicado devuelve `true`.
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args) {
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

private:
    //! @brief Inserción ordenada recursiva.
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

    //! @brief Libera todos los nodos y deja la lista vacía.
    void destruir_nodos() {
        scoped_lock<mutex> lock(m_mtx);
        Node* p = m_pRoot;
        while (p) { Node* nx = p->getNext(); delete p; p = nx; }
        m_pRoot = m_pTail = nullptr;
        m_size = 0;
    }
};

//! @brief Operador `<<` para imprimir la lista.
template <typename T, typename Comp>
ostream& operator<<(ostream& os, LinkedList<T, Comp>& list) {
    return os << list.toString();
}

//! @brief Operador `>>` que lee una línea en formato `[..]` y la inserta.
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
