#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <deque>
#include <mutex>
#include <functional>
#include <utility>
#include "general_iterator.h"
#include "basetrait.h"
#include "../foreach.h"
#include "../types.h"

using namespace std;

// ============================================================
// "Políticas" que definen el orden de recorrido
// Cada una es un template<Node> static void construir(deque<Node*>&, Node*)
// ============================================================

struct BinaryTreeForwardInorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(0));
        cola.push_back(n);
        construir(cola, n->getChild(1));
    }
};

struct BinaryTreeBackwardInorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(1));
        cola.push_back(n);
        construir(cola, n->getChild(0));
    }
};

struct BinaryTreeForwardPreorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        cola.push_back(n);
        construir(cola, n->getChild(0));
        construir(cola, n->getChild(1));
    }
};

struct BinaryTreeBackwardPreorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        cola.push_back(n);
        construir(cola, n->getChild(1));
        construir(cola, n->getChild(0));
    }
};

struct BinaryTreeForwardPostorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(0));
        construir(cola, n->getChild(1));
        cola.push_back(n);
    }
};

struct BinaryTreeBackwardPostorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(1));
        construir(cola, n->getChild(0));
        cola.push_back(n);
    }
};

// ============================================================
// Iterador unificado — reemplaza los 6 anteriores
// Policy::construir() llena la deque según el recorrido
// ============================================================
template <typename Container, typename Policy>
class BinaryTreeIterator
    : public general_iterator<Container, BinaryTreeIterator<Container, Policy>> {
    using MySelf = BinaryTreeIterator<Container, Policy>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_cola;

    void avanzar() {
        if (!m_cola.empty()) { this->m_pNode = m_cola.front(); m_cola.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeIterator(Container* pC, Node* pRaiz)
        : Parent(pC, nullptr) { Policy::construir(m_cola, pRaiz); avanzar(); }

    MySelf& operator++() { avanzar(); return *this; }
};

// ============================================================
// Rango iterable — habilita for(auto& n : bt.inorder())
// Adquiere unique_lock<mutex> durante toda la iteracion
// IMPORTANTE: no usar dentro de metodos que ya tengan scoped_lock
//             (mutex no reentrante) — ver toString() en BinaryTree
// ============================================================
template <typename Container, typename Policy>
class BinaryTreeRange {
    using Iter = BinaryTreeIterator<Container, Policy>;
    unique_lock<mutex> m_lock;
    Iter               m_begin;
    Iter               m_end;
public:
    BinaryTreeRange(Container* pC, typename Container::Node* pRoot, mutex& mtx)
        : m_lock(mtx)
        , m_begin(pC, pRoot)
        , m_end(pC, nullptr)
    {}
    BinaryTreeRange(BinaryTreeRange&&)            = default;
    BinaryTreeRange(const BinaryTreeRange&)       = delete;
    BinaryTreeRange& operator=(BinaryTreeRange&&) = delete;

    Iter begin() { return m_begin; }
    Iter end()   { return m_end;   }
};

// ============================================================
// Traits helper
// ============================================================
template <typename T, typename CompTrait = AscendingTrait<T>>
struct BinaryTreeTraits : CompTrait {
    using value_type = T;
};

// ============================================================
// Árbol binario de búsqueda (BST) genérico con nodo dentro de container
// ============================================================
template <typename Traits>
class BinaryTree {
public:
    using value_type = typename Traits::value_type;
    using Comp       = typename Traits::Comp;
    struct Node;
    using NodePtr    = Node*;
    using MySelf     = BinaryTree<Traits>;
    template <typename, typename> friend class BinaryTreeRange;

    // Nodo anidado dentro del contenedor
    struct Node {
        using value_type = typename Traits::value_type;
        value_type m_data;
        Ref        m_ref;
        Node*      m_pChild[2];
        Node*      m_pParent;

        Node(const value_type& data, const Ref& ref,
             Node* izq = nullptr, Node* der = nullptr)
            : m_data(data), m_ref(ref), m_pParent(nullptr)
        { m_pChild[0] = izq; m_pChild[1] = der; }

        // Copy constructor
        Node(const Node& other)
            : m_data(other.m_data), m_ref(other.m_ref), m_pParent(nullptr)
        {
            m_pChild[0] = other.m_pChild[0] ? new Node(*other.m_pChild[0]) : nullptr;
            m_pChild[1] = other.m_pChild[1] ? new Node(*other.m_pChild[1]) : nullptr;
        }

        // Move constructor
        Node(Node&& other) noexcept
            : m_data(move(other.m_data)),
              m_ref(exchange(other.m_ref, Ref{})),
              m_pParent(nullptr)
        {
            m_pChild[0] = exchange(other.m_pChild[0], nullptr);
            m_pChild[1] = exchange(other.m_pChild[1], nullptr);
        }

        // Destructor en cascada: delete de un hijo dispara el suyo, eliminando todo el subárbol
        ~Node() { delete m_pChild[0]; delete m_pChild[1]; }

        value_type  getData()    const { return m_data; }
        value_type& getDataRef()       { return m_data; }
        void        setData(const value_type& d) { m_data = d; }
        Ref    getRef()     const { return m_ref; }
        Ref&   getRefRef()        { return m_ref; }
        void   setRef(Ref r)      { m_ref = r; }

        Node*  getChild(size_t pos)     const { return m_pChild[pos]; }
        Node*& getChildRef(size_t pos)        { return m_pChild[pos]; }
        void   setChild(size_t pos, Node* p)  { m_pChild[pos] = p; }

        Node*  getParent()              const { return m_pParent; }
        void   setParent(Node* p)             { m_pParent = p; }

        string to_string() const {
            ostringstream ss;
            ss << "Nodo(dato: " << m_data << ", ref: " << m_ref << ")";
            return ss.str();
        }

        friend ostream& operator<<(ostream& os, const Node& n) {
            return os << n.to_string();
        }

        friend istream& operator>>(istream& is, Node& n) {
            string linea;
            if (getline(is, linea)) {
                istringstream ss(linea);
                ss >> n.m_data >> n.m_ref;
            }
            return is;
        }
    };


    using forward_inorder_iterator    = BinaryTreeIterator<MySelf, BinaryTreeForwardInorderPolicy>;
    using backward_inorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeBackwardInorderPolicy>;
    using forward_preorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeForwardPreorderPolicy>;
    using backward_preorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeBackwardPreorderPolicy>;
    using forward_postorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeForwardPostorderPolicy>;
    using backward_postorder_iterator = BinaryTreeIterator<MySelf, BinaryTreeBackwardPostorderPolicy>;

protected:
    NodePtr        m_pRoot = nullptr;
    size_t         m_size  = 0;
    Comp           m_comp;
    mutable mutex  m_mtx;

public:
    BinaryTree() = default;

    // Constructor de copia
    BinaryTree(const BinaryTree& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        copiar_interno(m_pRoot, other.m_pRoot);
        m_size = other.m_size;
    }

    // Constructor de movimiento
    BinaryTree(BinaryTree&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_size  = exchange(other.m_size,  size_t{0});
    }

    // Destructor seguro: delete raíz dispara la cadena de destructores en todo el árbol
    ~BinaryTree() {
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;
        m_pRoot = nullptr;
    }

    void insert(const value_type& value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        insertar_interno(m_pRoot, value, ref);
    }

    size_t size()  const { return m_size; }
    bool   empty() const { return m_size == 0; }

    // --- Iteradores inorden ---
    forward_inorder_iterator  begin()    { return {this, m_pRoot}; }
    forward_inorder_iterator  end()      { return {this, nullptr}; }
    backward_inorder_iterator rbegin()   { return {this, m_pRoot}; }
    backward_inorder_iterator rend()     { return {this, nullptr}; }

    // --- Iteradores preorden ---
    forward_preorder_iterator  pre_begin()   { return {this, m_pRoot}; }
    forward_preorder_iterator  pre_end()     { return {this, nullptr}; }
    backward_preorder_iterator rpre_begin()  { return {this, m_pRoot}; }
    backward_preorder_iterator rpre_end()    { return {this, nullptr}; }

    // --- Iteradores postorden ---
    forward_postorder_iterator  post_begin()   { return {this, m_pRoot}; }
    forward_postorder_iterator  post_end()     { return {this, nullptr}; }
    backward_postorder_iterator rpost_begin()  { return {this, m_pRoot}; }
    backward_postorder_iterator rpost_end()    { return {this, nullptr}; }

    // --- metodos para: for(auto& n : bt.inorder()) ---
    auto inorder()          { return BinaryTreeRange<MySelf, BinaryTreeForwardInorderPolicy   >(this, m_pRoot, m_mtx); }
    auto reverse_inorder()  { return BinaryTreeRange<MySelf, BinaryTreeBackwardInorderPolicy  >(this, m_pRoot, m_mtx); }
    auto preorder()         { return BinaryTreeRange<MySelf, BinaryTreeForwardPreorderPolicy  >(this, m_pRoot, m_mtx); }
    auto reverse_preorder() { return BinaryTreeRange<MySelf, BinaryTreeBackwardPreorderPolicy >(this, m_pRoot, m_mtx); }
    auto postorder()        { return BinaryTreeRange<MySelf, BinaryTreeForwardPostorderPolicy >(this, m_pRoot, m_mtx); }
    auto reverse_postorder(){ return BinaryTreeRange<MySelf, BinaryTreeBackwardPostorderPolicy>(this, m_pRoot, m_mtx); }

    // --- Recorridos completos ---
    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }
    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }
    template <typename Func, typename... Args>
    void ForEachPreorder(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(pre_begin(), pre_end(), func, forward<Args>(args)...);
    }
    template <typename Func, typename... Args>
    void ForEachReversePreorder(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(rpre_begin(), rpre_end(), func, forward<Args>(args)...);
    }
    template <typename Func, typename... Args>
    void ForEachPostorder(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(post_begin(), post_end(), func, forward<Args>(args)...);
    }
    template <typename Func, typename... Args>
    void ForEachReversePostorder(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        ::ForEach(rpost_begin(), rpost_end(), func, forward<Args>(args)...);
    }

    // --- Búsqueda condicional ---
    template <typename Func, typename... Args>
    forward_inorder_iterator FirstThat(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }
    template <typename Func, typename... Args>
    backward_inorder_iterator ReverseFirstThat(Func func, Args&&... args) {
        scoped_lock<mutex> lock(m_mtx);
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }

    string toString() {
        scoped_lock<mutex> lock(m_mtx);
        ostringstream ss;
        ss << "[";
        bool primero = true;
        for (auto it = begin(); it != end(); ++it) {
            if (!primero) ss << ",";
            ss << (*it).to_string();
            primero = false;
        }
        ss << "]";
        return ss.str();
    }

    // Sobrecarga operador <<
    friend ostream& operator<<(ostream& os, MySelf& bt) {
        scoped_lock<mutex> lock(bt.m_mtx);
        os << bt.m_size << "\n";
        bt.escribir_interno(os, bt.m_pRoot);
        return os;
    }

    //Sobrecarga operador >>
    friend istream& operator>>(istream& is, MySelf& bt) {
        size_t n;
        is >> n;
        is.ignore();
        for (size_t i = 0; i < n; ++i) {
            value_type data; Ref ref;
            is >> data >> ref;
            is.ignore();
            bt.insert(data, ref);
        }
        return is;
    }

private:
    void insertar_interno(NodePtr& pNodo, const value_type& value, Ref ref) {
        if (!pNodo) {
            pNodo = new Node(value, ref);
            ++m_size;
            return;
        }
        size_t pos = !m_comp(value, pNodo->getDataRef());
        insertar_interno(pNodo->getChildRef(pos), value, ref);
    }

    void copiar_interno(NodePtr& dst, const NodePtr src) {
        if (!src) { dst = nullptr; return; }
        dst = new Node(src->getData(), src->getRef());
        copiar_interno(dst->getChildRef(0), src->getChild(0));
        copiar_interno(dst->getChildRef(1), src->getChild(1));
    }

    void escribir_interno(ostream& os, const Node* nodo) const {
        if (!nodo) return;
        os << nodo->getData() << " " << nodo->getRef() << "\n";
        escribir_interno(os, nodo->getChild(0));
        escribir_interno(os, nodo->getChild(1));
    }
};

void DemoBinaryTree();

#endif // __BINARY_TREE_H__
