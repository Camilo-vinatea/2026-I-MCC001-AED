//! @file doublelinkedlist.h
//! @brief Lista doblemente enlazada ordenada que hereda de `LinkedList`.
//! @details Comparte la lógica con `LinkedList`, pero añade el puntero
//!          `m_pPrev` y sobreescribe las inserciones para mantener ambos
//!          enlaces sincronizados.
//! @author Equipo MCC

#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include "linkedlist.h"

//! @class DoubleLinkedListBackwardIterator
//! @brief Iterador que recorre la DLL usando `m_pPrev`.
template <typename Container>
class DoubleLinkedListBackwardIterator
    : public general_iterator<Container, DoubleLinkedListBackwardIterator<Container>>
{
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    //! @brief Avanza retrocediendo en la lista.
    MySelf& operator++() {
        this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

//! @class DoubleLinkedList
//! @brief Lista doblemente enlazada ordenada.
//! @tparam T    Tipo de dato almacenado.
//! @tparam Comp Comparador (mismo contrato que en `LinkedList`).
template <typename T, typename Comp = less<T>>
class DoubleLinkedList : public LinkedList<T, Comp> {
public:
    using Parent = LinkedList<T, Comp>;

    //! @struct Node
    //! @brief Nodo con punteros `next` (heredado) y `prev` (propio).
    struct Node : public Parent::Node {
        using LLNode = typename Parent::Node;
        Node* m_pPrev;

        //! @brief Constructor con dato, referencia, next y prev opcionales.
        Node(T data, Ref ref, Node* next = nullptr, Node* prev = nullptr)
            : LLNode(data, ref, static_cast<LLNode*>(next)), m_pPrev(prev) {}

        //! @brief Sombrea el `getNext()` para devolver `DLL::Node*`.
        Node*  getNext()  const { return static_cast<Node*>(LLNode::m_pNext); }
        //! @brief Sombrea el `setNext()` con conversión de tipo.
        void   setNext(Node* p) { LLNode::m_pNext = static_cast<LLNode*>(p); }

        //! @brief Devuelve el nodo anterior.
        Node*  getPrev()  const { return m_pPrev; }
        //! @brief Devuelve referencia al puntero `prev`.
        Node*& getPrevRef()     { return m_pPrev; }
        //! @brief Establece el nodo anterior.
        void   setPrev(Node* p) { m_pPrev = p; }
    };

    using value_type        = T;
    using MySelf            = DoubleLinkedList<T, Comp>;
    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

    //! @brief Constructor por defecto.
    DoubleLinkedList() = default;

    //! @brief Constructor de copia: copia nodo por nodo.
    DoubleLinkedList(const DoubleLinkedList& other) {
        Node* p = static_cast<Node*>(other.m_pRoot);
        while (p) { push_back(p->getData(), p->getRef()); p = p->getNext(); }
    }

    //! @brief Constructor de movimiento.
    DoubleLinkedList(DoubleLinkedList&& other) noexcept : Parent(move(other)) {}

    //! @brief Destructor virtual por defecto (cadena de deletes en cascada).
    ~DoubleLinkedList() override = default;

    //! @brief Inserta al frente manteniendo `m_pPrev`.
    void push_front(value_type value, Ref ref) override {
        Node* p = new Node(value, ref);
        scoped_lock<mutex> lock(this->m_mtx);
        if (!this->m_size) {
            this->m_pRoot = p;
            this->m_pTail = p;
        } else {
            Node* raiz = static_cast<Node*>(this->m_pRoot);
            p->setNext(raiz);
            raiz->setPrev(p);
            this->m_pRoot = p;
        }
        ++this->m_size;
    }

    //! @brief Inserta al final manteniendo `m_pPrev`.
    void push_back(value_type value, Ref ref) override {
        Node* p = new Node(value, ref);
        scoped_lock<mutex> lock(this->m_mtx);
        if (!this->m_size) {
            this->m_pRoot = p;
            this->m_pTail = p;
        } else {
            Node* cola = static_cast<Node*>(this->m_pTail);
            cola->setNext(p);
            p->setPrev(cola);
            this->m_pTail = p;
        }
        ++this->m_size;
    }

    //! @brief Inserción ordenada con actualización de los dos enlaces.
    void insert(const value_type& value, Ref ref) override {
        scoped_lock<mutex> lock(this->m_mtx);
        Node* pNuevo = new Node(value, ref);
        Node* pAct   = static_cast<Node*>(this->m_pRoot);
        Node* pAnt   = nullptr;

        while (pAct && !this->m_comp(value, pAct->getDataRef())) {
            pAnt = pAct;
            pAct = pAct->getNext();
        }

        pNuevo->setNext(pAct);
        pNuevo->setPrev(pAnt);

        if (pAnt)
            pAnt->setNext(pNuevo);
        else
            this->m_pRoot = pNuevo;

        if (pAct)
            pAct->setPrev(pNuevo);
        else
            this->m_pTail = pNuevo;

        ++this->m_size;
    }

    //! @brief Iterador al primer nodo.
    forward_iterator  begin()  { return {this, static_cast<Node*>(this->m_pRoot)}; }
    //! @brief Iterador "uno más allá" del último.
    forward_iterator  end()    { return {this, nullptr}; }
    //! @brief Iterador inverso al último nodo.
    backward_iterator rbegin() { return {this, static_cast<Node*>(this->m_pTail)}; }
    //! @brief Iterador inverso al "previo al primero".
    backward_iterator rend()   { return {this, nullptr}; }

    //! @brief Recorrido hacia adelante con `unique_lock` durante toda la iteración.
    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        unique_lock<mutex> lock(this->m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    //! @brief Recorrido hacia atrás (`scoped_lock`).
    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args&&... args) {
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }

    //! @brief Primer nodo cuyo predicado es `true` (recorrido forward).
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args) {
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

    //! @brief Primer nodo cuyo predicado es `true` (recorrido inverso).
    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args&&... args) {
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }
};

#endif // __DOUBLELINKEDLIST_H__
