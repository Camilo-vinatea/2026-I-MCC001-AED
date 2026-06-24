// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "../types.h"
#include "BTreePage.h"

#define DEFAULT_BTREE_ORDER 3

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
       typedef CBTreePage <keyType, ObjIDType> BTNode;// useful shorthand
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/

public:
       //typedef Node iterator;
       typedef typename BTNode::lpfnForEach2    lpfnForEach2;
       typedef typename BTNode::lpfnForEach3    lpfnForEach3;
       typedef typename BTNode::lpfnFirstThat2  lpfnFirstThat2;
       typedef typename BTNode::lpfnFirstThat3  lpfnFirstThat3;
       typedef typename BTNode::Node            Node;

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
       void            ForEach( lpfnForEach2 lpfn, void *pExtra1 );
       void            ForEach( lpfnForEach3 lpfn, void *pExtra1, void *pExtra2);
       Node*           FirstThat( lpfnFirstThat2 lpfn, void *pExtra1 );
       Node*           FirstThat( lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2);
       //typedef               Node iterator;

protected:
       BTNode          m_Root;
       tree_height_t   m_Height;  // height of tree
       tree_order_t    m_Order;   // order of tree
       size_t          m_NumKeys; // number of keys
       bool            m_Unique;  // Accept the elements only once ?
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
       ObjIDType ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}

template <typename Traits>
void BTree<Traits>::ForEach(lpfnForEach2 lpfn, void *pExtra1)
{
       m_Root.ForEach(lpfn, 0, pExtra1);
}

template <typename Traits>
void BTree<Traits>::ForEach(lpfnForEach3 lpfn, void *pExtra1, void *pExtra2)
{
       m_Root.ForEach(lpfn, 0, pExtra1, pExtra2);
}

template <typename Traits>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(lpfnFirstThat2 lpfn, void *pExtra1)
{
       return m_Root.FirstThat(lpfn, 0, pExtra1);
}

template <typename Traits>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(lpfnFirstThat3 lpfn, void *pExtra1, void *pExtra2)
{
       return m_Root.FirstThat(lpfn, 0, pExtra1, pExtra2);
}

template <typename Traits>
void BTree<Traits>::Print(ostream &os){
       m_Root.Print(os);
}

void BTreeDemo();

#endif