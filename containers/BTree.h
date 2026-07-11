//! @file BTree.h
//! @brief Contenedor B-Tree con split en 3 vías, iteradores y concurrencia.
//! @details Define:
//!          - `BTreeTraits<K, V>`: trait del B-Tree (clave e ID de objeto).
//!          - `BTreeTraitDefault` = `BTreeTraits<btree_key_t>`.
//!          - `BTree<Traits>`: raíz lógica del árbol, expone:
//!            * Constructores por orden (`order`) y unicidad.
//!            * `Insert`, `Remove`, `Search` (todos con `scoped_lock`).
//!            * `Print`, `size`, `height`, `GetOrder`.
//!            * Iteradores forward (`begin/end`) y backward
//!              (`rbegin/rend`) que materializan la cola completa en orden.
//!            * `ForEach`, `ForEachInternal`, `FirstThat` que delegan en
//!              `CBTreePage::Call` (consume un `BTreeIterator`).
//!            * `mutable std::shared_mutex m_Mutex` para coordinar hilos.
//! @author Equipo MCC

#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include "../types.h"
#include "../foreach.h"
#include "BTreePage.h"

//! @brief Orden por defecto del B-Tree (3 → 2·orden+1 = 7 claves por nodo).
#define DEFAULT_BTREE_ORDER 3

//! @struct BTreeTraits
//! @brief Trait del B-Tree: tipo de clave y tipo de `ObjID` asociado.
//! @tparam K Tipo de la clave.
//! @tparam V Tipo del identificador del objeto (`obj_id_t` por defecto).
template <typename K, typename V = obj_id_t>
struct BTreeTraits{
    using KeyType   = K;   //!< Tipo de la clave.
    using ObjIDType = V;   //!< Tipo del `ObjID`.
};

//! @brief Trait por defecto con `btree_key_t` (definido en `types.h`).
using BTreeTraitDefault = BTreeTraits<btree_key_t>;

//! @class BTree
//! @brief B-Tree completo: combina `CBTreePage` raíz, mutex, e iteradores.
//! @tparam Traits Default = `BTreeTraitDefault`.
template <typename Traits = BTreeTraitDefault>
class BTree
{
    using keyType   = typename Traits::KeyType;
    using ObjIDType = typename Traits::ObjIDType;
    typedef CBTreePage<Traits> BTNode;

public:
    //! @brief Nodo del árbol (`tagNode<keyType, ObjIDType>`).
    typedef typename BTNode::Node Node;

    //! @brief Iterador forward (in-order ascendente).
    using forward_iterator  = BTreeIterator<BTree<Traits>, BTreeForwardInorderPolicy>;
    //! @brief Iterador backward (in-order descendente).
    using backward_iterator = BTreeIterator<BTree<Traits>, BTreeBackwardInorderPolicy>;

    //! @brief Iterador al primer nodo en orden ascendente (con lock).
    forward_iterator  begin() { std::scoped_lock lock(m_Mutex); return {this, &m_Root}; }
    //! @brief Iterador "uno más allá" del último.
    forward_iterator  end()   { std::scoped_lock lock(m_Mutex); return {this, nullptr}; }
    //! @brief Iterador al primer nodo en orden descendente.
    backward_iterator rbegin(){ return {this, &m_Root}; }
    //! @brief Iterador inverso al "previo al primero".
    backward_iterator rend()  { return {this, nullptr}; }

public:
    //! @brief Construye un B-Tree con el orden dado y modo de unicidad.
    BTree(tree_order_t order = DEFAULT_BTREE_ORDER, bool unique = true);
    //! @brief Destructor por defecto.
    ~BTree();

    //! @brief Inserta `(key, ObjID)`. `false` si la clave es duplicada.
    bool            Insert(const keyType key, const ObjIDType ObjID);
    //! @brief Elimina la clave. `false` si duplicado o no encontrado.
    bool            Remove(const keyType key, const ObjIDType ObjID);
    //! @brief Busca y devuelve el `ObjID` o -1 si no existe.
    ObjIDType       Search(const keyType key);
    //! @brief Número total de claves insertadas.
    size_t          size()  { return m_NumKeys; }
    //! @brief Altura del árbol.
    tree_height_t   height(){ return m_Height; }
    //! @brief Orden con el que se construyó el árbol.
    tree_order_t    GetOrder() { return m_Order; }
    //! @brief Impresión con sangrado por nivel.
    void            Print(ostream &os);

    //! @brief Recorre todos los nodos aplicando `func(nodo, args...)`.
    template <typename Func, typename ...Args> void ForEach(Func lpfn, Args&&... args);
    //! @brief Recorrido interno que recibe el nivel como segundo parámetro.
    template <typename Func, typename ...Args> void ForEachInternal(Func func, Args&&... args);
    //! @brief Primer nodo cuyo predicado devuelve `true` (devuelve puntero).
    template <typename Func, typename ...Args> Node* FirstThat(Func func, Args&&... args);

protected:
    BTNode          m_Root;                 //!< Página raíz del B-Tree.
    tree_height_t   m_Height;               //!< Altura (raíz = 1).
    tree_order_t    m_Order;                //!< Orden del árbol.
    size_t          m_NumKeys;              //!< Total de claves.
    bool            m_Unique;               //!< ¿Acepta duplicados?
    mutable std::shared_mutex m_Mutex;      //!< Mutex (lecturas compartidas posibles).
};

//! @brief Altura máxima esperada (reservada por la implementación).
const tree_height_t MaxHeight = 5;

//! @brief Constructor: crea la raíz con `2·order+1` slots.
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

//! @brief Inserta `(key, ObjID)`; `false` si la clave estaba duplicada.
template <typename Traits>
bool BTree<Traits>::Insert(const keyType key, ObjIDType ObjID)
{
    std::scoped_lock lock(m_Mutex);
    bt_ErrorCode error = m_Root.Insert(key, ObjID);
    if( error == bt_duplicate )
        return false;
    m_NumKeys++;
    if( error == bt_overflow ) {
        m_Root.SplitRoot();
        m_Height++;
    }
    return true;
}

//! @brief Elimina la clave; `false` si duplicado/no encontrado.
template <typename Traits>
bool BTree<Traits>::Remove(const keyType key, ObjIDType ObjID)
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

//! @brief Busca la clave y devuelve su `ObjID` (o -1).
template <typename Traits>
typename BTree<Traits>::ObjIDType BTree<Traits>::Search(const keyType key)
{
    std::scoped_lock lock(m_Mutex);
    ObjIDType ObjID = -1;
    m_Root.Search(key, ObjID);
    return ObjID;
}

//! @brief Recorre todo el árbol llamando `func(nodo, args...)` con lock.
template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEach(Func lpfn, Args&&... args)
{
    std::scoped_lock lock(m_Mutex);
    ::ForEach(forward_iterator(this, &m_Root), forward_iterator(this, nullptr),
              lpfn, std::forward<Args>(args)...);
}

//! @brief Recorrido interno: `func(nodo, nivel, args...)`.
template <typename Traits>
template <typename Func, typename... Args>
void BTree<Traits>::ForEachInternal(Func func, Args&&... args)
{
    std::scoped_lock lock(m_Mutex);
    m_Root.ForEach(func, 0, std::forward<Args>(args)...);
}

//! @brief Primer nodo que cumple el predicado.
template <typename Traits>
template <typename Func, typename... Args>
typename BTree<Traits>::Node *
BTree<Traits>::FirstThat(Func func, Args&&... args)
{
    std::scoped_lock lock(m_Mutex);
    return m_Root.FirstThat(func, 0, std::forward<Args>(args)...);
}

//! @brief Imprime el árbol con sangrado por nivel (usa `ForEachInternal`).
template <typename Traits>
void BTree<Traits>::Print(ostream &os){
    std::scoped_lock lock(m_Mutex);
    m_Root.ForEach([&os](Node& n, tree_height_t level) {
        for (auto i = 0; i < level; ++i) os << "\t";
        os << n.key << "->" << n.ObjID << "\n";
    }, 0);
}

//! @brief Demo del B-Tree con `Insert`, `Print`, iteradores y `ForEach`.
void BTreeDemo();

#endif //__BTREE_H__
