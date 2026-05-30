#ifndef __DOUBLELINKEDLIST_H__
#define __DOUBLELINKEDLIST_H__

#include "linkedlist.h"

// Iterador hacia atrás — recorre la lista por los punteros m_pPrev
template <typename Container>
class DoubleLinkedListBackwardIterator
    : public general_iterator<Container, DoubleLinkedListBackwardIterator<Container>> {
    using MySelf = DoubleLinkedListBackwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    MySelf& operator++() {
        this->m_pNode = this->m_pNode->getPrev();
        return *this;
    }
};

// Lista doblemente enlazada ordenada
// Hereda toda la lógica de LinkedList; solo agrega m_pPrev en el nodo
// y sobreescribe las mutaciones para mantener los enlaces bidireccionales.
template <typename T, typename Comp = less<T>>
class DoubleLinkedList : public LinkedList<T, Comp> {
public:
    using Parent = LinkedList<T, Comp>;

    // Nodo anidado: extiende el nodo de LinkedList con puntero al nodo anterior
    struct Node : public Parent::Node {
        using LLNode = typename Parent::Node;
        Node* m_pPrev;

        Node(T data, Ref ref, Node* next = nullptr, Node* prev = nullptr)
            : LLNode(data, ref, static_cast<LLNode*>(next)), m_pPrev(prev) {}

        // Sombrea getNext() del padre para devolver DLL::Node*
        Node*  getNext()  const { return static_cast<Node*>(LLNode::m_pNext); }
        void   setNext(Node* p) { LLNode::m_pNext = static_cast<LLNode*>(p); }

        Node*  getPrev()        const { return m_pPrev; }
        Node*& getPrevRef()           { return m_pPrev; }
        void   setPrev(Node* p)       { m_pPrev = p; }
    };

    using value_type        = T;
    using MySelf            = DoubleLinkedList<T, Comp>;
    using forward_iterator  = LinkedListForwardIterator<MySelf>;
    using backward_iterator = DoubleLinkedListBackwardIterator<MySelf>;

    DoubleLinkedList() = default;

    DoubleLinkedList(const DoubleLinkedList& other) {
        Node* p = static_cast<Node*>(other.m_pRoot);
        while (p) { push_back(p->getData(), p->getRef()); p = p->getNext(); }
    }

    DoubleLinkedList(DoubleLinkedList&& other) noexcept : Parent(move(other)) {}

    ~DoubleLinkedList() override = default;

    // Inserta al frente y actualiza m_pPrev del antiguo primer nodo
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

    // Inserta al final y actualiza m_pPrev del nuevo nodo
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

    // Inserción ordenada manteniendo enlaces bidireccionales
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

    forward_iterator  begin()   { return {this, static_cast<Node*>(this->m_pRoot)}; }
    forward_iterator  end()     { return {this, nullptr}; }
    backward_iterator rbegin()  { return {this, static_cast<Node*>(this->m_pTail)}; }
    backward_iterator rend()    { return {this, nullptr}; }

    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        unique_lock<mutex> lock(this->m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args&&... args) {
        scoped_lock<mutex> lock(this->m_mtx);
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args) {
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args&&... args) {
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }
};

#endif // __DOUBLELINKEDLIST_H__
