#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef> // size_t
#include <string>
#include <sstream>
#include <shared_mutex> // shared_mutex
#include "general_iterator.h"
#include "util.h"
#include "types.h"
#include "foreach.h"
#include "basetrait.h"

using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Linked List Node
template <typename T>
class LLNode{
    using value_type = T;
    using Node       = LLNode<T>;
private:
    value_type m_data;
    Ref        m_ref;
    Node      *m_pNext;
public:
    LLNode(){}
    LLNode(value_type data, Ref ref, Node *pNext = nullptr) 
           : m_data(data), m_ref(ref), m_pNext(pNext) {}
    virtual ~LLNode() {}

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }
    Node*           getNext() const { return m_pNext; }
    Node*&          getNextRef()    { return m_pNext; }
    void            setNext(Node *pNext) { m_pNext = pNext; }
};

template <typename T>
ostream &operator<<(ostream &os, const LLNode<T> &node){
    return os << "(" <<node.getData() << ", " << node.getRef() << ")";
}

template <typename T>
struct BaseLinkedListTrait : public BaseContainerTrait<T, LLNode<T>>{
};

template <typename T>
struct AscendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Traits>
class LinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = LinkedList<Traits>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_tail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable shared_mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other){ // Copy constructor
    }
    LinkedList(LinkedList &&other){ // Move constructor
    }
    LinkedList& operator=(const LinkedList &other){ // Copy assignment operator
    }
    LinkedList& operator=(LinkedList &&other){ // Move assignment operator
    }
    
    virtual        ~LinkedList() {}
    // virtual void    push_front(value_type value, Ref ref);
    // virtual void    pop_front();
    // virtual void    push_back(value_type value, Ref ref);
    // virtual void    pop_back();
private:
            void    internal_insert(Node* &pParent, const value_type &value, Ref ref);
public:
    virtual void    insert(const value_type &value, Ref ref);
    
    // virtual Node& operator[](size_t index);
    virtual size_t  size() const { return m_size; }
    virtual string  toString();

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        unique_lock<shared_mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template <typename Traits>
void LinkedList<Traits>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        pPrev = new Node(value, ref, pPrev);
        m_size++;
        if(pPrev == m_pRoot)
            m_tail = pPrev;
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Traits>
void LinkedList<Traits>::insert(const value_type &value, Ref ref){
    internal_insert(m_pRoot, value, ref);
}

template <typename Traits>
string  LinkedList<Traits>::toString() {
    stringstream ss;
    Node *pNode = m_pRoot;
    ss << "[";
    if( m_size > 0 ){
        for( size_t i = 0 ; i < size()-1 ; ++i ){
            ss << *pNode << ",";
            pNode = pNode->getNext();
        }
        ss << *pNode;
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, LinkedList<Traits>& list){
    return os << list.toString();
}


#endif // __LINKEDLIST_H__