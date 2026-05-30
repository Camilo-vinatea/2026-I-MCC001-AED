#ifndef __BINARY_TREE_H__
#define __BINARY_TREE_H__

#include <iostream>
#include <cstddef>
#include <string>
#include <sstream>
#include <deque>
#include <mutex>
#include <utility>
#include "general_iterator.h"
#include "basetrait.h"
#include "../foreach.h"
#include "../types.h"

using namespace std;

// ============================================================
// BinaryTreeNode
// ============================================================
template <typename T>
class BinaryTreeNode {
public:
    using value_type = T;
    using Node       = BinaryTreeNode<T>;
    using NodePtr    = Node*;

protected:
    value_type m_data;
    Ref        m_ref;
    NodePtr    m_pChild[2];
    NodePtr    m_pParent;

public:
    BinaryTreeNode(const value_type& data, const Ref& ref,
                   NodePtr left = nullptr, NodePtr right = nullptr)
        : m_data(data), m_ref(ref), m_pParent(nullptr)
    {
        m_pChild[0] = left;
        m_pChild[1] = right;
    }

    // Deep copy: recursively clone subtrees
    BinaryTreeNode(const BinaryTreeNode& other)
        : m_data(other.m_data), m_ref(other.m_ref), m_pParent(nullptr)
    {
        m_pChild[0] = other.m_pChild[0] ? new Node(*other.m_pChild[0]) : nullptr;
        m_pChild[1] = other.m_pChild[1] ? new Node(*other.m_pChild[1]) : nullptr;
    }

    // Move: nullify source via exchange
    BinaryTreeNode(BinaryTreeNode&& other) noexcept
        : m_data(move(other.m_data)),
          m_ref(exchange(other.m_ref, Ref{})),
          m_pParent(nullptr)
    {
        m_pChild[0] = exchange(other.m_pChild[0], nullptr);
        m_pChild[1] = exchange(other.m_pChild[1], nullptr);
    }

    // Recursive: deleting a child triggers its destructor, cascading down the subtree
    ~BinaryTreeNode() {
        delete m_pChild[0];
        delete m_pChild[1];
    }

    value_type  getData()    const { return m_data; }
    value_type& getDataRef()       { return m_data; }
    void        setData(const value_type& data) { m_data = data; }
    Ref         getRef()     const { return m_ref; }
    Ref&        getRefRef()        { return m_ref; }
    void        setRef(Ref ref)    { m_ref = ref; }

    NodePtr     getChild(size_t pos)    const { return m_pChild[pos]; }
    NodePtr&    getChildRef(size_t pos)       { return m_pChild[pos]; }
    void        setChild(size_t pos, NodePtr p) { m_pChild[pos] = p; }

    NodePtr     getParent()              const { return m_pParent; }
    void        setParent(NodePtr p)           { m_pParent = p; }

    string to_string() const {
        ostringstream ss;
        ss << "Node(data: " << m_data << ", ref: " << m_ref << ")";
        return ss.str();
    }

    friend ostream& operator<<(ostream& os, const BinaryTreeNode& node) {
        return os << node.to_string();
    }

    // Disk format: "data ref"
    friend istream& operator>>(istream& is, BinaryTreeNode& node) {
        string line;
        if (getline(is, line)) {
            istringstream ss(line);
            ss >> node.m_data >> node.m_ref;
        }
        return is;
    }
};

// ============================================================
// Traits
// ============================================================
template <typename T>
struct BaseBinaryTreeListTrait : public BaseContainerTrait<T, BinaryTreeNode<T>> {};

template <typename T>
struct AscendingBinaryTreeListTrait : public BaseBinaryTreeListTrait<T>,
                                      public AscendingTrait<T> {};

template <typename T>
struct DescendingBinaryTreeListTrait : public BaseBinaryTreeListTrait<T>,
                                       public DescendingTrait<T> {};

// ============================================================
// Iterators
// All use the same pattern:
//   - Constructor with (Container*, Node* root) builds deque + calls advance()
//   - Constructor with (Container*, nullptr) produces end() iterator (empty deque)
//   - operator++ advances the deque
// ============================================================

// --- Inorder Forward: LNR (left -> node -> right) ---
template <typename Container>
class BinaryTreeForwardInorderIterator
    : public general_iterator<Container, BinaryTreeForwardInorderIterator<Container>> {
    using MySelf = BinaryTreeForwardInorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_queue;

    void build(Node* n) {
        if (!n) return;
        build(n->getChild(0));
        m_queue.push_back(n);
        build(n->getChild(1));
    }
    void advance() {
        if (!m_queue.empty()) { this->m_pNode = m_queue.front(); m_queue.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeForwardInorderIterator(Container* pC, Node* pRoot)
        : Parent(pC, nullptr) { build(pRoot); advance(); }

    MySelf& operator++() { advance(); return *this; }
};

// --- Inorder Backward: RNL (right -> node -> left) ---
template <typename Container>
class BinaryTreeBackwardInorderIterator
    : public general_iterator<Container, BinaryTreeBackwardInorderIterator<Container>> {
    using MySelf = BinaryTreeBackwardInorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_queue;

    void build(Node* n) {
        if (!n) return;
        build(n->getChild(1));
        m_queue.push_back(n);
        build(n->getChild(0));
    }
    void advance() {
        if (!m_queue.empty()) { this->m_pNode = m_queue.front(); m_queue.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeBackwardInorderIterator(Container* pC, Node* pRoot)
        : Parent(pC, nullptr) { build(pRoot); advance(); }

    MySelf& operator++() { advance(); return *this; }
};

// --- Preorder Forward: NLR (node -> left -> right) ---
template <typename Container>
class BinaryTreeForwardPreorderIterator
    : public general_iterator<Container, BinaryTreeForwardPreorderIterator<Container>> {
    using MySelf = BinaryTreeForwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_queue;

    void build(Node* n) {
        if (!n) return;
        m_queue.push_back(n);
        build(n->getChild(0));
        build(n->getChild(1));
    }
    void advance() {
        if (!m_queue.empty()) { this->m_pNode = m_queue.front(); m_queue.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeForwardPreorderIterator(Container* pC, Node* pRoot)
        : Parent(pC, nullptr) { build(pRoot); advance(); }

    MySelf& operator++() { advance(); return *this; }
};

// --- Preorder Backward: NRL (node -> right -> left) ---
template <typename Container>
class BinaryTreeBackwardPreorderIterator
    : public general_iterator<Container, BinaryTreeBackwardPreorderIterator<Container>> {
    using MySelf = BinaryTreeBackwardPreorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_queue;

    void build(Node* n) {
        if (!n) return;
        m_queue.push_back(n);
        build(n->getChild(1));
        build(n->getChild(0));
    }
    void advance() {
        if (!m_queue.empty()) { this->m_pNode = m_queue.front(); m_queue.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeBackwardPreorderIterator(Container* pC, Node* pRoot)
        : Parent(pC, nullptr) { build(pRoot); advance(); }

    MySelf& operator++() { advance(); return *this; }
};

// --- Postorder Forward: LRN (left -> right -> node) ---
template <typename Container>
class BinaryTreeForwardPostorderIterator
    : public general_iterator<Container, BinaryTreeForwardPostorderIterator<Container>> {
    using MySelf = BinaryTreeForwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_queue;

    void build(Node* n) {
        if (!n) return;
        build(n->getChild(0));
        build(n->getChild(1));
        m_queue.push_back(n);
    }
    void advance() {
        if (!m_queue.empty()) { this->m_pNode = m_queue.front(); m_queue.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeForwardPostorderIterator(Container* pC, Node* pRoot)
        : Parent(pC, nullptr) { build(pRoot); advance(); }

    MySelf& operator++() { advance(); return *this; }
};

// --- Postorder Backward: RLN (right -> left -> node) ---
template <typename Container>
class BinaryTreeBackwardPostorderIterator
    : public general_iterator<Container, BinaryTreeBackwardPostorderIterator<Container>> {
    using MySelf = BinaryTreeBackwardPostorderIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_queue;

    void build(Node* n) {
        if (!n) return;
        build(n->getChild(1));
        build(n->getChild(0));
        m_queue.push_back(n);
    }
    void advance() {
        if (!m_queue.empty()) { this->m_pNode = m_queue.front(); m_queue.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    BinaryTreeBackwardPostorderIterator(Container* pC, Node* pRoot)
        : Parent(pC, nullptr) { build(pRoot); advance(); }

    MySelf& operator++() { advance(); return *this; }
};

// ============================================================
// BinaryTree
// ============================================================
template <typename Traits>
class BinaryTree {
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using NodePtr    = Node*;
    using MySelf     = BinaryTree<Traits>;

    using forward_inorder_iterator    = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator   = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator   = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator  = BinaryTreeBackwardPreorderIterator<MySelf>;
    using forward_postorder_iterator  = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;

protected:
    NodePtr        m_pRoot = nullptr;
    size_t         m_size  = 0;
    Comp           m_comp;
    mutable mutex  m_mtx;

public:
    BinaryTree() = default;

    // Deep copy: internal_copy clones each node recursively
    BinaryTree(const BinaryTree& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        internal_copy(m_pRoot, other.m_pRoot);
        m_size = other.m_size;
    }

    // Move: transfer root ownership, leave source empty
    BinaryTree(BinaryTree&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_size  = exchange(other.m_size,  size_t{0});
    }

    // BinaryTreeNode destructor is recursive: delete root cascades to all nodes
    ~BinaryTree() {
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;
        m_pRoot = nullptr;
    }

    void insert(const value_type& value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        internal_insert(m_pRoot, value, ref);
    }

    size_t size()  const { return m_size; }
    bool   empty() const { return m_size == 0; }

    // --- Inorder (default): ascending BST order ---
    forward_inorder_iterator  begin()   { return {this, m_pRoot}; }
    forward_inorder_iterator  end()     { return {this, nullptr}; }
    backward_inorder_iterator rbegin()  { return {this, m_pRoot}; }
    backward_inorder_iterator rend()    { return {this, nullptr}; }

    // --- Preorder ---
    forward_preorder_iterator  pre_begin()   { return {this, m_pRoot}; }
    forward_preorder_iterator  pre_end()     { return {this, nullptr}; }
    backward_preorder_iterator rpre_begin()  { return {this, m_pRoot}; }
    backward_preorder_iterator rpre_end()    { return {this, nullptr}; }

    // --- Postorder ---
    forward_postorder_iterator  post_begin()   { return {this, m_pRoot}; }
    forward_postorder_iterator  post_end()     { return {this, nullptr}; }
    backward_postorder_iterator rpost_begin()  { return {this, m_pRoot}; }
    backward_postorder_iterator rpost_end()    { return {this, nullptr}; }

    // --- ForEach ---
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

    // --- FirstThat ---
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
        bool first = true;
        for (auto it = begin(); it != end(); ++it) {
            if (!first) ss << ",";
            ss << (*it).to_string();
            first = false;
        }
        ss << "]";
        return ss.str();
    }

    // Write: count + preorder. Inserting back in preorder reconstructs identical BST structure.
    friend ostream& operator<<(ostream& os, MySelf& bt) {
        scoped_lock<mutex> lock(bt.m_mtx);
        os << bt.m_size << "\n";
        bt.internal_write(os, bt.m_pRoot);
        return os;
    }

    // Read: count header + "data ref" lines -> insert each
    friend istream& operator>>(istream& is, MySelf& bt) {
        size_t n;
        is >> n;
        is.ignore();
        for (size_t i = 0; i < n; ++i) {
            value_type data;
            Ref ref;
            is >> data >> ref;
            is.ignore();
            bt.insert(data, ref);
        }
        return is;
    }

private:
    void internal_insert(NodePtr& pNode, const value_type& value, Ref ref) {
        if (!pNode) {
            pNode = new Node(value, ref);
            ++m_size;
            return;
        }
        size_t pos = !m_comp(value, pNode->getDataRef());
        internal_insert(pNode->getChildRef(pos), value, ref);
    }

    void internal_copy(NodePtr& dst, const NodePtr src) {
        if (!src) { dst = nullptr; return; }
        dst = new Node(src->getData(), src->getRef());
        internal_copy(dst->getChildRef(0), src->getChild(0));
        internal_copy(dst->getChildRef(1), src->getChild(1));
    }

    void internal_write(ostream& os, const Node* node) const {
        if (!node) return;
        os << node->getData() << " " << node->getRef() << "\n";
        internal_write(os, node->getChild(0));
        internal_write(os, node->getChild(1));
    }
};

void DemoBinaryTree();

#endif // __BINARY_TREE_H__
