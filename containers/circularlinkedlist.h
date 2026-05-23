#ifndef __CIRCULARLINKEDLIST_H__
#define __CIRCULARLINKEDLIST_H__

#include "linkedlist.h"

using namespace std;

/*
Forward Iterator (lista circular)

Permite recorrer la estructura de forma circular.
El recorrido vuelve al inicio cuando se completa el ciclo.
*/
template <typename Container>
class CircularLinkedListForwardIterator :
    public general_iterator<Container, CircularLinkedListForwardIterator<Container>>{

    using MySelf = CircularLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;

    typename Container::Node* m_pStart;

public:
    CircularLinkedListForwardIterator(
        Container* pContainer,
        typename Container::Node* pNode
    )
    : Parent(pContainer, pNode), m_pStart(pNode) {}

    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        if(this->m_pNode == m_pStart)
            this->m_pNode = nullptr;
        return *this;
    }
};

/*
Circular Linked List

Estructura enlazada donde el último nodo apunta nuevamente al primero,
formando un ciclo continuo.
*/
template <typename Traits>
class CircularLinkedList :
    public LinkedList<Traits>{

public:
    using Parent = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node = typename Traits::Node;

    using forward_iterator =
        CircularLinkedListForwardIterator<CircularLinkedList>;

public:
    CircularLinkedList() : Parent(){}

    virtual ~CircularLinkedList(){
        if(this->m_size > 0)
            this->m_pTail->setNext(nullptr);
    }

    /*
    Inserción ordenada en lista circular

    Rompe temporalmente el enlace circular para reutilizar la inserción
    ordenada de LinkedList, luego restaura la circularidad.
    */
    virtual void insert(
        const value_type& value,
        Ref ref
    ) override {
        scoped_lock<mutex> lock(this->m_mtx);

        if(this->m_size > 0)
            this->m_pTail->setNext(nullptr);

        Parent::internal_insert(this->m_pRoot, value, ref, nullptr);

        this->m_pTail->setNext(this->m_pRoot);
    }

    // Acceso al nodo raíz
    Node* getRoot(){
        return this->m_pRoot;
    }

    // Iterador desde el inicio
    forward_iterator begin(){
        return forward_iterator(
            this,
            this->m_pRoot
        );
    }

    // Fin lógico de iteración
    forward_iterator end(){
        return forward_iterator(
            this,
            nullptr
        );
    }
};

#endif //__CIRCULARLINKEDLIST_H__