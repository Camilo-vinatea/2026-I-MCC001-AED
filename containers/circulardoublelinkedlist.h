#ifndef __CIRCULARDOUBLELINKEDLIST_H__
#define __CIRCULARDOUBLELINKEDLIST_H__

#include "doublelinkedlist.h"

using namespace std;

/*
Forward Iterator (circular doble)

Recorre la lista en dirección forward de forma circular continua.
No tiene centinela: el recorrido se controla externamente por conteo.
*/
template <typename Container>
class CircularDoubleLinkedListForwardIterator :
    public general_iterator<Container,
    CircularDoubleLinkedListForwardIterator<Container>>{

    using MySelf = CircularDoubleLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;

    typename Container::Node* m_pStart;

public:
    CircularDoubleLinkedListForwardIterator(
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
Backward Iterator (circular doble)

Recorre la lista desde el tail hacia root, deteniéndose al completar un ciclo.
*/
template <typename Container>
class CircularDoubleLinkedListBackwardIterator :
    public general_iterator<Container,
    CircularDoubleLinkedListBackwardIterator<Container>>{

    using MySelf = CircularDoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;

    typename Container::Node* m_pStart;

public:
    CircularDoubleLinkedListBackwardIterator(
        Container* pContainer,
        typename Container::Node* pNode
    )
    : Parent(pContainer, pNode), m_pStart(pNode) {}

    MySelf& operator++(){
        this->m_pNode =
            ((typename Container::Node*)this->m_pNode)->getPrev();
        if(this->m_pNode == m_pStart)
            this->m_pNode = nullptr;
        return *this;
    }
};

/*
Circular Double Linked List

Extiende DoubleLinkedList para formar una estructura circular bidireccional.
El último nodo conecta con el primero y viceversa.
La inserción es ordenada y reutiliza la lógica de DoubleLinkedList.
*/
template <typename Traits>
class CircularDoubleLinkedList :
    public DoubleLinkedList<Traits>{

public:
    using Parent     = DoubleLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;

    using forward_iterator =
        CircularDoubleLinkedListForwardIterator<CircularDoubleLinkedList>;

    using backward_iterator =
        CircularDoubleLinkedListBackwardIterator<CircularDoubleLinkedList>;

public:
    CircularDoubleLinkedList() : Parent(){}

    virtual ~CircularDoubleLinkedList(){
        if(this->m_size > 0){
            ((Node*)this->m_pTail)->setNext(nullptr);
            ((Node*)this->m_pRoot)->setPrev(nullptr);
        }
    }

    /*
    Inserción ordenada en lista doble circular

    Rompe temporalmente los enlaces circulares para reutilizar la inserción
    ordenada de DoubleLinkedList, luego restaura la circularidad en ambas
    direcciones.
    */
    virtual void insert(const value_type& value, Ref ref) override {
        scoped_lock<mutex> lock(this->m_mtx);

        if(this->m_size > 0){
            ((Node*)this->m_pTail)->setNext(nullptr);
            ((Node*)this->m_pRoot)->setPrev(nullptr);
        }

        Parent::internal_insert(
            (Node*&)this->m_pRoot,
            value,
            ref,
            nullptr
        );

        ((Node*)this->m_pTail)->setNext((Node*)this->m_pRoot);
        ((Node*)this->m_pRoot)->setPrev((Node*)this->m_pTail);
    }

    forward_iterator begin(){
        return forward_iterator(this, (Node*)this->m_pRoot);
    }

    forward_iterator end(){
        return forward_iterator(this, nullptr);
    }

    backward_iterator rbegin(){
        return backward_iterator(this, (Node*)this->m_pTail);
    }

    backward_iterator rend(){
        return backward_iterator(this, nullptr);
    }
};

#endif // __CIRCULARDOUBLELINKEDLIST_H__
