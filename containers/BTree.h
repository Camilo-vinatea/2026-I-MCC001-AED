// btree.h

#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <deque>
#include <algorithm>
#include "../types.h"
#include "../foreach.h"
#include "BTreePage.h"
#include "general_iterator.h"

#define DEFAULT_BTREE_ORDER 3

struct BTreeForwardInorderPolicy {
    template <typename Page, typename Node>
    static void construir(std::deque<Node*>& cola, Page* p) {
        if (!p) return;
        p->ForEach([&cola](Node& n, tree_height_t){ cola.push_back(&n); }, 0);
    }
};

struct BTreeBackwardInorderPolicy {
    template <typename Page, typename Node>
    static void construir(std::deque<Node*>& cola, Page* p) {
        BTreeForwardInorderPolicy::construir(cola, p);
        std::reverse(cola.begin(), cola.end());
    }
};

template <typename Container, typename Policy>
class BTreeIterator: public general_iterator<Container, BTreeIterator<Container, Policy>> {
    using MySelf = BTreeIterator<Container, Policy>;
    using Parent = general_iterator<Container, MySelf>;

public:
    using Node = typename Container::Node;

    template <typename Page>
    BTreeIterator(Container* pC, Page* pRaiz)
        : Parent(pC, nullptr) { Policy::construir(m_cola, pRaiz); avanzar(); }
    BTreeIterator(Container* pC, std::nullptr_t)
        : Parent(pC, nullptr) {}
    MySelf& operator++() { avanzar(); return *this; }

private:
    std::deque<Node*> m_cola;
    void avanzar() {
        if (!m_cola.empty()) { this->m_pNode = m_cola.front(); m_cola.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
};


template <typename K, typename V = obj_id_t>
struct BTreeTraits{
       using KeyType = K;
       using ObjIDType = V;
};

using BTreeTraitDefault = BTreeTraits<btree_key_t>;

template <typename Traits = BTreeTraitDefault>
class BTree 
// this is the full version of the BTree
{
       using keyType = typename Traits::KeyType;
       using ObjIDType = typename Traits::ObjIDType;
       typedef CBTreePage<Traits> BTNode;
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/

public:
       typedef typename BTNode::Node            Node;

       using forward_iterator  = BTreeIterator<BTree<Traits>, BTreeForwardInorderPolicy>;
       using backward_iterator = BTreeIterator<BTree<Traits>, BTreeBackwardInorderPolicy>;

       forward_iterator  begin()  {  std::scoped_lock lock(m_Mutex); return {this, &m_Root}; }
       forward_iterator  end()    {  std::scoped_lock lock(m_Mutex); return {this, nullptr}; }
       backward_iterator rbegin() { return {this, &m_Root}; }
       backward_iterator rend()   { return {this, nullptr}; }

public:
       BTree(tree_order_t order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTree();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       bool            Insert (const keyType key, const ObjIDType ObjID);
       bool            Remove (const keyType key, const ObjIDType ObjID);
       ObjIDType       Search (const keyType key);
       size_t          size()  { return m_NumKeys; }
       tree_height_t   height() { return m_Height;      }
       tree_order_t    GetOrder() { return m_Order;     }
       void            Print (ostream &os);
       template <typename Func, typename ...Args> void ForEachInternal(Func func, Args&&... args);
       template <typename Func, typename ...Args> Node* FirstThat(Func func, Args&&... args);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;
       tree_height_t   m_Height;  // height of tree
       tree_order_t    m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
       mutable std::shared_mutex m_Mutex;
};

const tree_height_t MaxHeight = 5;
template <typename Traits>
BTree<Traits>::BTree(tree_order_t order, bool unique)
                               : m_Root(2 * order  + 1, unique),
                                 m_Order(order),
                                 m_NumKeys(0),
                                 m_Unique(unique)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Traits>
BTree<Traits>::~BTree()
{
}

template <typename Traits>
bool BTree<Traits>::Insert(const keyType key, ObjIDType ObjID)
{
       std::scoped_lock lock(m_Mutex);
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Traits>
bool BTree<Traits>::Remove (const keyType key, ObjIDType ObjID)
{
       std::scoped_lock lock(m_Mutex);
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename BTree<Traits>::ObjIDType BTree<Traits>::Search (const keyType key)
{
       std::scoped_lock lock(m_Mutex);
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}

template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEachInternal(Func func, Args&&... args)
{
       std::scoped_lock lock(m_Mutex);
       m_Root.ForEach(func, 0, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(Func func, Args&&... args)
{
       std::scoped_lock lock(m_Mutex);
       return m_Root.FirstThat(func, 0, std::forward<Args>(args)...);
}

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       std::scoped_lock lock(m_Mutex);
       m_Root.ForEach([&os](Node& n, tree_height_t level) {
              for (auto i = 0; i < level; ++i) os << "\t";
              os << n.key << "->" << n.ObjID << "\n";
       }, 0);
}

void BTreeDemo();

#endif //__BTREE_H__