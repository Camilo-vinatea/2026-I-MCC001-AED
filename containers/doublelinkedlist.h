#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <mutex>
#include "linkedlist.h"

using namespace std;

//////////////////////////////////////////////////////////////
// Double Linked List Node
//
// Nodo doblemente enlazado.
// Hereda datos y next desde LLNode y agrega prev.
//////////////////////////////////////////////////////////////
template <typename T>
class DLLNode : public LLNode<T>{
public:
    using value_type = T;
    using Node = DLLNode<T>;

private:
    Node* m_pPrev;

public:
    DLLNode(){}

    DLLNode(value_type data, Ref ref,
            Node* pNext = nullptr,
            Node* pPrev = nullptr)
        : LLNode<T>(data, ref, pNext), m_pPrev(pPrev){}

    virtual ~DLLNode(){}

    //////////////////////////////////////////////////////////
    // Reutilizamos el puntero next del padre
    // pero devolviendo DLLNode*
    //////////////////////////////////////////////////////////
    Node* getNext() const{
        return (Node*)LLNode<T>::getNext();
    }

    Node*& getNextRef(){
        return (Node*&)LLNode<T>::getNextRef();
    }

    void setNext(Node* pNext){
        LLNode<T>::setNext(pNext);
    }

    //////////////////////////////////////////////////////////
    // Prev
    //////////////////////////////////////////////////////////
    Node* getPrev() const{
        return m_pPrev;
    }

    Node*& getPrevRef(){
        return m_pPrev;
    }

    void setPrev(Node* pPrev){
        m_pPrev = pPrev;
    }
};

//////////////////////////////////////////////////////////////
// Forward Iterator
//
// Igual al de LinkedList.
// Solo avanza usando next.
//////////////////////////////////////////////////////////////
template <typename Container>
class DoubleLinkedListForwardIterator :
    public general_iterator<Container,
    DoubleLinkedListForwardIterator<Container>>{

    using MySelf = DoubleLinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;

    using Parent::Parent;

public:
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

//////////////////////////////////////////////////////////////
// Backward Iterator
//
// Recorre la lista desde atrás hacia adelante.
//////////////////////////////////////////////////////////////
template <typename Container>
class DoubleLinkedListBackwardIterator :
    public general_iterator<Container,
    DoubleLinkedListBackwardIterator<Container>>{

    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;

    using Parent::Parent;

public:
    MySelf& operator++(){
        this->m_pNode =
            ((typename Container::Node*)this->m_pNode)->getPrev();
        return *this;
    }
};

//////////////////////////////////////////////////////////////
// Traits
//////////////////////////////////////////////////////////////
template <typename T>
struct BaseDoubleLinkedListTrait :
    public BaseContainerTrait<T, DLLNode<T>>{

};

template <typename T>
struct AscendingDoubleLinkedListTrait :
    public BaseDoubleLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingDoubleLinkedListTrait :
    public BaseDoubleLinkedListTrait<T>{
    using Comp = greater<T>;
};

//////////////////////////////////////////////////////////////
// DoubleLinkedList
//
// Reutiliza gran parte de LinkedList.
// Solo adapta lo necesario para manejar prev.
//////////////////////////////////////////////////////////////
template <typename Traits>
class DoubleLinkedList : public LinkedList<Traits>{
public:
    using Parent = LinkedList<Traits>;
    using value_type = typename Traits::value_type;
    using Node = typename Traits::Node;
    using Comp = typename Traits::Comp;

    using forward_iterator =
        DoubleLinkedListForwardIterator<DoubleLinkedList>;

    using backward_iterator =
        DoubleLinkedListBackwardIterator<DoubleLinkedList>;

private:
    //////////////////////////////////////////////////////////////
    // Insert interno
    //
    // Reutiliza la lógica recursiva del LinkedList
    // pero agrega actualización de prev.
    //////////////////////////////////////////////////////////////
    void internal_insert(Node*& pCurrent,
                         Node* pPrev,
                         const value_type& value,
                         Ref ref){

        if(!pCurrent || this->m_comp(value, pCurrent->getDataRef())){

            Node* pNew = new Node(value, ref, pCurrent, pPrev);

            if(pCurrent)
                pCurrent->setPrev(pNew);

            pCurrent = pNew;

            if(!pPrev)
                this->m_pRoot = pNew;

            if(!pNew->getNext())
                this->m_pTail = pNew;

            ++this->m_size;
            return;
        }

        internal_insert(
            (Node*&)pCurrent->getNextRef(),
            pCurrent,
            value,
            ref
        );
    }

public:
    //////////////////////////////////////////////////////////////
    // Constructor
    //////////////////////////////////////////////////////////////
    DoubleLinkedList() : Parent(){}

    //////////////////////////////////////////////////////////////
    // Copy constructor
    //
    // Reutiliza push_back().
    //////////////////////////////////////////////////////////////
    DoubleLinkedList(const DoubleLinkedList& other){

        Node* pTemp = (Node*)other.m_pRoot;

        while(pTemp){
            push_back(
                pTemp->getData(),
                pTemp->getRef()
            );
            pTemp = (Node*)pTemp->getNext();
        }
    }

    //////////////////////////////////////////////////////////////
    // Destructor
    //
    // Reutiliza destructor del padre.
    //////////////////////////////////////////////////////////////
    virtual ~DoubleLinkedList(){}

    //////////////////////////////////////////////////////////////
    // Push front
    //////////////////////////////////////////////////////////////
    virtual void push_front(value_type value, Ref ref){

        scoped_lock<mutex> lock(this->m_mtx);

        Node* pTemp =
            new Node(
                value,
                ref,
                (Node*)this->m_pRoot,
                nullptr
            );

        if(this->m_pRoot)
            ((Node*)this->m_pRoot)->setPrev(pTemp);

        this->m_pRoot = pTemp;

        if(this->m_size == 0)
            this->m_pTail = pTemp;

        ++this->m_size;
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args){
        return ::FirstThat(
            begin(),
            end(),
            func,
            std::forward<Args>(args)...
        );
    }

    //////////////////////////////////////////////////////////////
    // Push back
    //////////////////////////////////////////////////////////////
    virtual void push_back(value_type value, Ref ref){

        scoped_lock<mutex> lock(this->m_mtx);

        Node* pTemp =
            new Node(
                value,
                ref,
                nullptr,
                (Node*)this->m_pTail
            );

        if(this->m_size == 0){
            this->m_pRoot = pTemp;
            this->m_pTail = pTemp;
        }
        else{
            ((Node*)this->m_pTail)->setNext(pTemp);
            this->m_pTail = pTemp;
        }

        ++this->m_size;
    }

    //////////////////////////////////////////////////////////////
    // Insert
    //
    // Mantiene misma lógica recursiva que LinkedList
    // pero corrige prev.
    //////////////////////////////////////////////////////////////
    virtual void insert(const value_type& value, Ref ref){

        scoped_lock<mutex> lock(this->m_mtx);

        internal_insert(
            (Node*&)this->m_pRoot,
            nullptr,
            value,
            ref
        );
    }

    //////////////////////////////////////////////////////////////
    // Iteradores forward
    //////////////////////////////////////////////////////////////
    forward_iterator begin(){
        return forward_iterator(this, this->m_pRoot);
    }

    forward_iterator end(){
        return forward_iterator(this, nullptr);
    }

    //////////////////////////////////////////////////////////////
    // Iteradores backward
    //////////////////////////////////////////////////////////////
    backward_iterator rbegin(){
        return backward_iterator(this, this->m_pTail);
    }

    backward_iterator rend(){
        return backward_iterator(this, nullptr);
    }
};

//////////////////////////////////////////////////////////////
// operator<<
//
// Reutiliza toString() heredado.
//////////////////////////////////////////////////////////////
template <typename Traits>
ostream& operator<<(ostream& os,
                    DoubleLinkedList<Traits>& list){
    return os << list.toString();
}

//////////////////////////////////////////////////////////////
// operator>>
//
// Reutiliza push_back().
//////////////////////////////////////////////////////////////
template <typename Traits>
istream& operator>>(istream& is,
                    DoubleLinkedList<Traits>& list){

    using value_type =
        typename DoubleLinkedList<Traits>::value_type;

    string line;

    getline(is, line);

    for(char& c : line){
        if(c == '[' || c == ']' ||
           c == '(' || c == ')' ||
           c == ','){
            c = ' ';
        }
    }

    value_type value;
    Ref ref;

    stringstream ss(line);

    while(ss >> value >> ref){
        list.push_back(value, ref);
    }

    return is;
}

#endif // __DOUBLELINKEDLIST_H__