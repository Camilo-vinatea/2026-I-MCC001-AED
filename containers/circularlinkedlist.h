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

    using Parent::Parent;

private:
    bool m_firstPass = true;

public:
    CircularLinkedListForwardIterator(
        Container* pContainer,
        typename Container::Node* pNode
    )
    : Parent(pContainer, pNode){}

    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
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

    /*
    Inserción en lista circular

    Mantiene la propiedad de ciclo entre el último y el primer nodo.
    */
    virtual void insert(
        const value_type& value,
        Ref ref
    ){
        scoped_lock<mutex> lock(this->m_mtx);
        Node* pNew =
            new Node(value, ref);

        // caso: lista vacía
        if(this->m_size == 0){
            this->m_pRoot = pNew;
            this->m_pTail = pNew;

            pNew->setNext(pNew);
        }
        else{
            this->m_pTail->setNext(pNew);
            pNew->setNext(this->m_pRoot);
            this->m_pTail = pNew;
        }

        ++this->m_size;
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