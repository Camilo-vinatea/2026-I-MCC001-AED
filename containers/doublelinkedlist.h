#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <mutex>
#include "linkedlist.h"

using namespace std;

/*
Double Linked List

Extiende una lista enlazada simple para permitir recorrido en ambas direcciones.
*/
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

    // Siguiente nodo
    Node* getNext() const{
        return (Node*)LLNode<T>::getNext();
    }

    Node*& getNextRef(){
        return (Node*&)LLNode<T>::getNextRef();
    }

    void setNext(Node* pNext){
        LLNode<T>::setNext(pNext);
    }

    // Nodo anterior
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

/*
Iterador forward

Recorre la lista desde el inicio hasta el final.
*/
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

/*
Iterador backward

Recorre la lista desde el final hacia el inicio.
*/
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

/*
Traits

Define tipo de dato y criterio de ordenamiento.
*/
template <typename T>
struct BaseDoubleLinkedListTrait :
    public BaseContainerTrait<T, DLLNode<T>>{};

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

/*
DoubleLinkedList

Lista enlazada doble con soporte para recorrido bidireccional
e inserción ordenada.
*/
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

protected:

    void internal_insert(Node*& pNode,
                         const value_type& value,
                         Ref ref,
                         Node* pPrev = nullptr) override {
        if(!pNode || this->m_comp(value, pNode->getDataRef())){
            Node* pNew = new Node(value, ref, pNode, pPrev);
            if(pNode)
                pNode->setPrev(pNew);
            pNode = pNew;
            if(!pPrev)
                this->m_pRoot = pNew;
            if(!pNew->getNext())
                this->m_pTail = pNew;
            ++this->m_size;
            return;
        }

        internal_insert(
            (Node*&)pNode->getNextRef(),
            value,
            ref,
            pNode
        );
    }

public:

    DoubleLinkedList() : Parent(){}

    // Copia de lista
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

    virtual ~DoubleLinkedList(){}

    // Inserción al inicio
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

    // Inserción al final
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

    // Iteradores forward
    forward_iterator begin(){
        return forward_iterator(this, this->m_pRoot);
    }

    forward_iterator end(){
        return forward_iterator(this, nullptr);
    }

    // Iteradores backward
    backward_iterator rbegin(){
        return backward_iterator(this, this->m_pTail);
    }

    backward_iterator rend(){
        return backward_iterator(this, nullptr);
    }
};

template <typename Traits>
ostream& operator<<(ostream& os, DoubleLinkedList<Traits>& list){
    return os << static_cast<LinkedList<Traits>&>(list);
}

template <typename Traits>
istream& operator>>(istream& is, DoubleLinkedList<Traits>& list){
    return is >> static_cast<LinkedList<Traits>&>(list);
}

#endif //__DOUBLELINKEDLIST_H__