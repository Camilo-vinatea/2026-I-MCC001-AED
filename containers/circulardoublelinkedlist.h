#ifndef __CIRCULARDOUBLELINKEDLIST_H__
#define __CIRCULARDOUBLELINKEDLIST_H__

#include "doublelinkedlist.h"

using namespace std;

//////////////////////////////////////////////////////////////
// Circular Double Linked List
//////////////////////////////////////////////////////////////
template <typename Traits>
class CircularDoubleLinkedList :
    public DoubleLinkedList<Traits>{

public:
    using Parent = DoubleLinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node = typename Traits::Node;

public:
    CircularDoubleLinkedList() : Parent(){}

    //////////////////////////////////////////////////////////
    // Insert adaptado para circularidad doble
    //////////////////////////////////////////////////////////
    virtual void insert(
        const value_type& value,
        Ref ref
    ){

        scoped_lock<mutex> lock(this->m_mtx);

        Node* pNew =
            new Node(value, ref);

        // lista vacía
        if(this->m_size == 0){

            this->m_pRoot = pNew;
            this->m_pTail = pNew;

            pNew->setNext(pNew);
            pNew->setPrev(pNew);
        }
        else{

            pNew->setNext(this->m_pRoot);
            pNew->setPrev(this->m_pTail);

            this->m_pTail->setNext(pNew);
            this->m_pRoot->setPrev(pNew);

            this->m_pTail = pNew;
        }

        ++this->m_size;
    }
};

#endif // __CIRCULARDOUBLELINKEDLIST_H__