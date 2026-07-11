//! @file BTreePage.h
//! @brief Página del B-Tree (`CBTreePage`), políticas, iterador y algoritmos
//!        auxiliares (búsqueda binaria, inserción/remoción en arreglos).
//! @details Contiene:
//!          - `binary_search`, `insert_at`, `remove` (helpers sobre arreglos).
//!          - `BTreeForwardInorderPolicy` / `BTreeBackwardInorderPolicy`:
//!            políticas de recorrido que delegan la iteración al árbol.
//!          - `BTreeIterator<Container, Policy>`: iterador con metadato
//!            de nivel; consume la cola materializada por la política.
//!          - `tagNode<K,V>`: nodo con clave, `ObjID` y contador de uso.
//!          - `CBTreePage<Traits>`: página del B-Tree con `Insert`,
//!            `Remove`, `Search`, `SplitRoot/SplitChild`, redistribuciones
//!            (`RedistributeL2R`/`R2L`), `Merge`/`MergeRoot`, `Print`
//!            y los motores de recorrido `ForEach`/`FirstThat` que
//!            delegan en un único `Call` que consume el iterador.
//!          - `enum bt_ErrorCode`: códigos retornados por las operaciones.
//! @author Equipo MCC

#ifndef CBTreePage_H
#define CBTreePage_H
#include <vector>
#include <iostream>
#include <assert.h>
#include <type_traits>
#include <deque>
#include <algorithm>
#include "../types.h"
#include "general_iterator.h"

//! @brief Búsqueda binaria en un arreglo cualquiera (`first` incluido,
//!        `last` excluido). Devuelve la posición donde debería ir `object`.
//! @tparam Container Contenedor con `operator[]` (p.ej. `std::vector`).
//! @tparam ObjType   Tipo comparable con el contenido.
template <typename Container, typename ObjType>
btree_pos_t binary_search(Container& container, btree_pos_t first, btree_pos_t last, ObjType &object)
{
    if( first >= last )
        return first;
    while( first < last )
    {
        btree_pos_t mid = (first+last)/2;
        if( object == (ObjType)container[mid] )
            return mid;
        if( object > (ObjType)container[mid] )
            first = mid+1;
        else
            last  = mid;
    }
    if( object <= (ObjType)container[first] )
        return first;
    return last;
}

//! @brief Inserta `object` en `container[pos]` desplazando los siguientes
//!        hacia la derecha.
template <typename Container, typename ObjType>
void insert_at(Container& container, const ObjType &object, btree_pos_t pos)
{
    bree_size_t size = container.size();
    for(auto i = size-2 ; i >= pos ; i--)
        container[i+1] = container[i];
    container[pos] = object;
}

//! @brief Quita el elemento en `container[pos]` desplazando a la izquierda.
template <typename Container>
void remove(Container& container, btree_pos_t pos)
{
    bree_size_t size = container.size();
    for(auto i = pos+1 ; i < size ; ++i)
        container[i-1] = container[i];
}

/* ======================== Forward declaration ======================= */
//! @brief Declaración adelantada de `BTree` (usa `CBTreePage`).
template <typename Traits>
class BTree;

/* ==================== Políticas de recorrido ======================== */
//! @struct BTreeForwardInorderPolicy
//! @brief Recorre el B-Tree en in-order ascendente, almacenando
//!        `(Node*, nivel)` en una cola.
//! @note Es amiga de `CBTreePage` para acceder directamente a `m_Keys`
//!       y `m_SubPages` sin pasar por `ForEach` (rompe la circularidad).
struct BTreeForwardInorderPolicy {
    //! @brief Llena `cola` con `(nodo, nivel)` en orden ascendente.
    template <typename Page, typename Node>
    static void construir(std::deque<std::pair<Node*, tree_height_t>>& cola,
                          Page* p, tree_height_t nivel = 0) {
        if (!p) return;
        for (btree_pos_t i = 0; i < p->m_KeyCount; ++i) {
            construir<Page, Node>(cola, p->m_SubPages[i], nivel + 1);
            cola.push_back({&p->m_Keys[i], nivel});
        }
        construir<Page, Node>(cola, p->m_SubPages[p->m_KeyCount], nivel + 1);
    }
};

//! @struct BTreeBackwardInorderPolicy
//! @brief Igual que la forward, pero invierte la cola para ir descendente.
struct BTreeBackwardInorderPolicy {
    //! @brief Llena la cola y la invierte con `std::reverse`.
    template <typename Page, typename Node>
    static void construir(std::deque<std::pair<Node*, tree_height_t>>& cola,
                          Page* p, tree_height_t nivel = 0) {
        BTreeForwardInorderPolicy::construir<Page, Node>(cola, p, nivel);
        std::reverse(cola.begin(), cola.end());
    }
};

/* ======================== Iterador ================================== */
//! @class BTreeIterator
//! @brief Iterador uniforme para el B-Tree que expone además `level()`.
//! @tparam Container `BTree<Traits>`.
//! @tparam Policy    `BTreeForwardInorderPolicy` o `Backward...`.
template <typename Container, typename Policy>
class BTreeIterator : public general_iterator<Container, BTreeIterator<Container, Policy>> {
    using MySelf = BTreeIterator<Container, Policy>;
    using Parent = general_iterator<Container, MySelf>;

public:
    using Node = typename Container::Node;

    //! @brief Construye materializando la cola en orden `Policy`.
    template <typename Page>
    BTreeIterator(Container* pC, Page* pRaiz, tree_height_t nivel = 0)
        : Parent(pC, nullptr) {
        Policy::template construir<Page, Node>(m_cola, pRaiz, nivel);
        avanzar();
    }
    //! @brief Constructor "fin" (cola vacía).
    BTreeIterator(Container* pC, std::nullptr_t)
        : Parent(pC, nullptr) {}

    //! @brief Avanza al siguiente nodo.
    MySelf& operator++() { avanzar(); return *this; }

    //! @brief Devuelve el nivel (altura/profundidad) del nodo actual.
    tree_height_t level() const { return m_nivel; }

private:
    std::deque<std::pair<Node*, tree_height_t>> m_cola; //!< Cola materializada.
    tree_height_t m_nivel = 0;                          //!< Nivel del nodo actual.

    //! @brief Saca el siguiente par `(nodo, nivel)`.
    void avanzar() {
        if (!m_cola.empty()) {
            this->m_pNode = m_cola.front().first;
            m_nivel       = m_cola.front().second;
            m_cola.pop_front();
        }
        else
            this->m_pNode = nullptr;
    }
};

using namespace std;

//! @enum bt_ErrorCode
//! @brief Códigos retornados por las operaciones de `CBTreePage`.
//! @var bt_ok          Operación exitosa.
//! @var bt_overflow    Nodo saturado (se debe dividir/propagar).
//! @var bt_underflow   Nodo por debajo del mínimo (redistribuir/fusionar).
//! @var bt_duplicate   Inserción duplicada con `m_Unique == true`.
//! @var bt_nofound     Clave inexistente.
//! @var bt_rootmerged  Se fusionaron páginas bajando la altura del árbol.
enum bt_ErrorCode { bt_ok, bt_overflow, bt_underflow, bt_duplicate, bt_nofound, bt_rootmerged };

/* ============================ tagNode ================================ */
//! @struct tagNode
//! @brief Elemento almacenado en un B-Tree: clave, `ObjID` y contador de uso.
//! @tparam keyType    Tipo de la clave.
//! @tparam ObjIDType  Tipo del identificador del objeto.
template <typename keyType, typename ObjIDType>
struct tagNode
{
    keyType                 key;        //!< Clave de búsqueda.
    ObjIDType               ObjID;      //!< ID del objeto asociado.
    use_count_t             UseCounter; //!< Veces consultada la clave.

    //! @brief Constructor.
    tagNode(const keyType &_key, ObjIDType _ObjID)
        : key(_key), ObjID(_ObjID), UseCounter(0) {}
    //! @brief Constructor por defecto.
    tagNode()               {}

    //! @brief Conversión implícita a `keyType` para comparaciones.
    operator keyType        ()     { return key; }
    //! @brief Devuelve el contador de uso.
    use_count_t             GetUseCounter() { return UseCounter; }
};

/* ========================== CBTreePage =============================== */
//! @class CBTreePage
//! @brief Página (nodo) del B-Tree con split en 3 vías, redistribuciones
//!        y fusión (`Merge`).
//! @tparam Traits Debe exponer `KeyType` y `ObjIDType`.
template <typename Traits>
class CBTreePage
{
    template <typename T> friend class BTree;
    friend struct BTreeForwardInorderPolicy;

    using keyType   = typename Traits::KeyType;
    using ObjIDType = typename Traits::ObjIDType;
    typedef CBTreePage<Traits>          BTPage;
    typedef tagNode<keyType, ObjIDType> Node;

public:
    //! @brief Constructor: inicializa con `maxKeys` y modo de unicidad.
    CBTreePage(btree_pos_t maxKeys, bool unique = true);
    //! @brief Destructor; libera los sub-páginas en cascada.
    virtual ~CBTreePage();

    //! @brief Inserta un par `(key, ObjID)` en la página (recursivo).
    //! @return `bt_duplicate`, `bt_overflow` o `bt_ok`.
    bt_ErrorCode Insert(const keyType &key, const ObjIDType ObjID);
    //! @brief Elimina una clave; retorna `bt_underflow`, `bt_rootmerged`,
    //!        `bt_nofound` o `bt_ok`.
    bt_ErrorCode Remove(const keyType &key, const ObjIDType ObjID);
    //! @brief Busca `key` y devuelve su `ObjID` (incrementa `UseCounter`).
    bool         Search(const keyType &key, obj_id_t &ObjID);
    //! @brief Impresión recursiva de claves e IDs.
    void         Print(ostream &os);

    //! @brief Recorre la página aplicando `func(node, level, args...)`.
    template <typename Func, typename ...Args> void ForEach(Func func, tree_height_t level, Args&&... args);
    //! @brief Primera página cuyo predicado es verdadero.
    template <typename Func, typename ...Args> Node* FirstThat(Func func, tree_height_t level, Args&&... args);

protected:
    btree_pos_t  m_MinKeys;              //!< Mínimo de claves permitido.
    btree_pos_t  m_MaxKeys;              //!< Máximo de claves permitido.
    tree_order_t m_MaxKeysForChilds;     //!< Máximo para los hijos (≠ raíz).
    bool         m_Unique;               //!< ¿Acepta duplicados?
    bool         m_isRoot;               //!< ¿Es la raíz?
    vector<Node>       m_Keys;            //!< Claves de la página.
    vector<BTPage *>   m_SubPages;        //!< Páginas hijas.
    btree_pos_t  m_KeyCount;             //!< Número de claves usadas.

    //! @brief Inicializa los vectores `m_Keys` y `m_SubPages`.
    void  Create();
    //! @brief Borra recursivamente todas las sub-páginas.
    void  Reset();
    //! @brief `Reset()` + `delete this` (usado en fusiones).
    void  Destroy() { Reset(); delete this; }
    //! @brief Reinicia `m_KeyCount` a 0 sin tocar memoria.
    void  clear();

    //! @brief Redistribución: elige hermano más rico y mueve claves.
    bool  Redistribute1(btree_pos_t &pos);
    //! @brief Redistribución entre tres vecinos tras verificar bajo flujo.
    bool  Redistribute2(btree_pos_t pos);
    //! @brief Mueve claves de derecha a izquierda.
    void  RedistributeR2L(btree_pos_t pos);
    //! @brief Mueve claves de izquierda a derecha.
    void  RedistributeL2R(btree_pos_t pos);

    //! @brief Aplica `Redistribute1` o, si falla, `Redistribute2`.
    bool  TreatUnderflow(btree_pos_t &pos)
    {     return Redistribute1(pos) || Redistribute2(pos); }

    //! @brief Fusiona tres páginas adyacentes en una.
    bt_ErrorCode Merge(btree_pos_t pos);
    //! @brief Fusiona las páginas de la raíz bajando la altura.
    bt_ErrorCode MergeRoot();
    //! @brief Divide la página (`pos`) en dos mitades; sube la mediana.
    void  SplitChild(btree_pos_t pos);

    //! @brief Devuelve el primer nodo hoja (recursivo).
    Node &GetFirstNode();

    //! @brief `true` si excede `m_MaxKeys`.
    bool Overflow()  { return m_KeyCount > m_MaxKeys; }
    //! @brief `true` si está por debajo del mínimo.
    bool Underflow() { return m_KeyCount < MinNumberOfKeys(); }
    //! @brief `true` si llegó al máximo (`>=`).
    bool IsFull()    { return m_KeyCount >= m_MaxKeys; }
    //! @brief Mínimo de claves que debe tener un nodo (≈ 2·max/3).
    btree_pos_t  MinNumberOfKeys()  { return 2*m_MaxKeys/3.0; }
    //! @brief Celdas libres = máximo - usadas.
    btree_pos_t  GetFreeCells()  { return m_MaxKeys - m_KeyCount; }
    //! @brief Referencia al contador (necesario para `++`).
    btree_pos_t& NumberOfKeys()  { return m_KeyCount; }
    //! @brief Lectura del contador.
    btree_pos_t  GetNumberOfKeys()  { return m_KeyCount; }
    //! @brief `true` si esta página es la raíz.
    bool IsRoot()  { return m_MaxKeysForChilds != m_MaxKeys; }
    //! @brief Ajusta `m_MaxKeysForChilds` (marca como raíz).
    void SetMaxKeysForChilds(tree_order_t orderforchilds){ m_MaxKeysForChilds = orderforchilds;}
    //! @brief Celdas libres del subárbol izquierdo a `pos`.
    btree_pos_t GetFreeCellsOnLeft(btree_pos_t pos);
    //! @brief Celdas libres del subárbol derecho a `pos`.
    btree_pos_t GetFreeCellsOnRight(btree_pos_t pos);

private:
    //! @brief Divide la raíz en tres sub-páginas (`SplitRoot`).
    bool SplitRoot();
    //! @brief Divide un vector temporal en tres páginas equilibradas.
    void SplitPageInto3(vector<Node>   & tmpKeys,
                        vector<BTPage *>  & SubPages,
                        BTPage           *& pChild1,
                        BTPage           *& pChild2,
                        BTPage           *& pChild3,
                        Node        & oi1,
                        Node        & oi2);
    //! @brief Mueve las claves/hijos de `pChildPage` a vectores auxiliares.
    void MovePage(BTPage *  pChildPage, vector<Node> &tmpKeys, vector<BTPage *> &tmpSubPages);
    //! @brief Motor único que recorre la página consumiendo un iterador
    //!        materializado; usado tanto por `ForEach` como por `FirstThat`.
    template <typename Func, typename ...Args> Node* Call(Func func, tree_height_t level, Args&&... args);
};

//! @brief Construye la página reservando `m_MaxKeys+1` slots.
template <typename Traits>
CBTreePage<Traits>::CBTreePage(btree_pos_t maxKeys, bool unique)
    : m_MaxKeys(maxKeys), m_Unique(unique), m_KeyCount(0)
{
    Create();
    SetMaxKeysForChilds(m_MaxKeys);
}

//! @brief Destructor: libera todas las sub-páginas.
template <typename Traits>
CBTreePage<Traits>::~CBTreePage()
{
    Reset();
}

//! @brief Inserción: si es hoja `insert_at`; si no, recursión + manejo de overflow.
template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Insert(const keyType& key, const ObjIDType ObjID)
{
    btree_pos_t pos = binary_search(m_Keys, 0, m_KeyCount, key);
    bt_ErrorCode error = bt_ok;

    if( pos < m_KeyCount && (keyType)m_Keys[pos] == key && m_Unique)
        return bt_duplicate;

    if( !m_SubPages[pos] ) // hoja
    {
        ::insert_at(m_Keys, Node(key, ObjID), pos);
        NumberOfKeys()++;
        if( Overflow() )
            return bt_overflow;
        return bt_ok;
    }
    // recursión + propagación de overflow
    error = m_SubPages[pos]->Insert(key, ObjID);
    if( error == bt_overflow )
    {
        if( !Redistribute1(pos) )
            SplitChild(pos);
        if( Overflow() )
            return bt_overflow;
        return bt_ok;
    }
    return bt_ok;
}

//! @brief Redistribución elegida entre hermanos cuando hay overflow o underflow.
//! @return `true` si pudo redistribuir, `false` si requiere split/merge.
template <typename Traits>
bool CBTreePage<Traits>::Redistribute1(btree_pos_t &pos)
{
    if( m_SubPages[pos]->Underflow() )
    {
        btree_pos_t nkol = 0, nkor = 0;
        if( pos > 0 ) nkol = m_SubPages[pos-1]->NumberOfKeys();
        if( pos < NumberOfKeys() ) nkor = m_SubPages[pos+1]->NumberOfKeys();

        if( nkol > nkor ){
            if( m_SubPages[pos-1]->NumberOfKeys() > m_SubPages[pos-1]->MinNumberOfKeys() )
                RedistributeL2R(pos-1);
            else {
                if( pos == NumberOfKeys() ) --pos;
                return false;
            }
        } else {
            if( m_SubPages[pos+1]->NumberOfKeys() > m_SubPages[pos+1]->MinNumberOfKeys() )
                RedistributeR2L(pos+1);
            else {
                if( pos == 0 ) ++pos;
                return false;
            }
        }
    }
    else // overflow
    {
        btree_pos_t fcol = GetFreeCellsOnLeft(pos);
        btree_pos_t fcor = GetFreeCellsOnRight(pos);

        if( !fcol && !fcor && m_SubPages[pos]->IsFull() )
            return false;
        if( fcol > fcor )
            RedistributeR2L(pos);
        else
            RedistributeL2R(pos);
    }
    return true;
}

//! @brief Redistribución entre tres vecinos cuando uno queda bajo mínimo.
template <typename Traits>
bool CBTreePage<Traits>::Redistribute2(btree_pos_t pos)
{
    assert( pos > 0 && pos < NumberOfKeys()  );
    assert( m_SubPages[pos-1] != 0 && m_SubPages[pos] != 0 && m_SubPages[pos+1] != 0 );
    assert( m_SubPages[pos-1]->Underflow() ||
            m_SubPages[ pos ]->Underflow() ||
            m_SubPages[pos+1]->Underflow() );

    if( m_SubPages[pos-1]->Underflow() )
    {
        RedistributeR2L(pos+1);
        RedistributeR2L(pos);
        if( m_SubPages[pos-1]->Underflow() ) return false;
    }
    else if( m_SubPages[pos+1]->Underflow() )
    {
        RedistributeL2R(pos-1);
        RedistributeL2R(pos);
        if( m_SubPages[pos+1]->Underflow() ) return false;
    }
    else
    {
        RedistributeL2R(pos-1);
        RedistributeR2L(pos+1);
        if( m_SubPages[pos]->Underflow() ) return false;
    }
    return true;
}

//! @brief Mueve claves de la página `pos` (derecha) a `pos-1` (izquierda).
template <typename Traits>
void CBTreePage<Traits>::RedistributeR2L(btree_pos_t pos)
{
    BTPage  *pSource = m_SubPages[pos],
            *pTarget = m_SubPages[pos-1];

    while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
          pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
    {
        ::insert_at(pTarget->m_Keys, m_Keys[pos-1], pTarget->NumberOfKeys()++);
        ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[0], pTarget->NumberOfKeys());
        m_Keys[pos-1] = pSource->m_Keys[0];
        ::remove(pSource->m_Keys    , 0);
        ::remove(pSource->m_SubPages, 0);
        pSource->NumberOfKeys()--;
    }
}

//! @brief Mueve claves de la página `pos` (izquierda) a `pos+1` (derecha).
template <typename Traits>
void CBTreePage<Traits>::RedistributeL2R(btree_pos_t pos)
{
    BTPage  *pSource = m_SubPages[pos],
            *pTarget = m_SubPages[pos+1];
    while(pSource->GetNumberOfKeys() > pSource->MinNumberOfKeys() &&
          pTarget->GetNumberOfKeys() < pSource->GetNumberOfKeys() )
    {
        ::insert_at(pTarget->m_Keys, m_Keys[pos], 0);
        ::insert_at(pTarget->m_SubPages, pSource->m_SubPages[pSource->NumberOfKeys()], 0);
        pTarget->NumberOfKeys()++;
        m_Keys[pos] = pSource->m_Keys[pSource->NumberOfKeys()-1];
        pSource->NumberOfKeys()--;
    }
}

//! @brief Divide la página `pos` en dos mitades; sube la mediana a `m_Keys[pos]`.
template <typename Traits>
void CBTreePage<Traits>::SplitChild(btree_pos_t pos)
{
    BTPage  *pChild1 = 0, *pChild2 = 0;
    if( pos > 0 )
        if( m_SubPages[pos-1]->IsFull() )
        { pChild1 = m_SubPages[pos-1]; pChild2 = m_SubPages[pos--]; }
    if( pos < GetNumberOfKeys() )
        if( m_SubPages[pos+1]->IsFull() )
        { pChild1 = m_SubPages[pos]; pChild2 = m_SubPages[pos+1]; }

    vector<Node> tmpKeys;
    vector<BTPage *> tmpSubPages;

    MovePage(pChild1, tmpKeys, tmpSubPages);
    tmpKeys.push_back(m_Keys[pos]);
    MovePage(pChild2, tmpKeys, tmpSubPages);

    BTPage *pChild3 = 0;
    Node oi1, oi2;
    SplitPageInto3(tmpKeys, tmpSubPages, pChild1, pChild2, pChild3, oi1, oi2);

    m_Keys[pos]    = oi1;
    m_SubPages[pos] = pChild1;
    ::insert_at(m_Keys, oi2, pos+1);
    ::insert_at(m_SubPages, pChild2, pos+1);
    NumberOfKeys()++;

    m_SubPages[pos+2] = pChild3;
}

//! @brief Divide el vector temporal en 3 páginas nuevas y devuelve las dos
//!        claves que subirán al padre.
template <typename Traits>
void CBTreePage<Traits>::SplitPageInto3(vector<Node>& tmpKeys,
                                        vector<BTPage *>  & tmpSubPages,
                                        BTPage*                   &     pChild1,
                                        BTPage*                   &     pChild2,
                                        BTPage*                   &     pChild3,
                                        Node                & oi1,
                                        Node                & oi2)
{
    assert(tmpKeys.size() >= 8);
    assert(tmpSubPages.size() >= 9);
    if( !pChild1 )
        pChild1 = new BTPage(m_MaxKeysForChilds, m_Unique);

    pChild1->clear();
    btree_pos_t nKeys = (tmpKeys.size()-2)/3;
    btree_pos_t i = 0;
    for( ; i < nKeys; ++i ) {
        pChild1->m_Keys[i] = tmpKeys[i];
        pChild1->m_SubPages[i] = tmpSubPages[i];
        pChild1->NumberOfKeys()++;
    }
    pChild1->m_SubPages[i] = tmpSubPages[i];

    oi1 = tmpKeys[i++];

    if( !pChild2 )
        pChild2 = new BTPage(m_MaxKeysForChilds, m_Unique);
    pChild2->clear();
    nKeys += (tmpKeys.size()-2)/3 + 1;
    btree_pos_t j = 0;
    for(; i < nKeys; ++i, ++j ) {
        pChild2->m_Keys[j] = tmpKeys[i];
        pChild2->m_SubPages[j] = tmpSubPages[i];
        pChild2->NumberOfKeys()++;
    }
    pChild2->m_SubPages[j] = tmpSubPages[i];

    oi2 = tmpKeys[i++];

    if( !pChild3 )
        pChild3 = new BTPage(m_MaxKeysForChilds, m_Unique);
    pChild3->clear();
    nKeys = (btree_pos_t)tmpKeys.size();
    for(j = 0; i < nKeys; ++i, ++j) {
        pChild3->m_Keys[j] = tmpKeys[i];
        pChild3->m_SubPages[j] = tmpSubPages[i];
        pChild3->NumberOfKeys()++;
    }
    pChild3->m_SubPages[j] = tmpSubPages[i];
}

//! @brief Divide la raíz cuando se desborda la primera vez.
template <typename Traits>
bool CBTreePage<Traits>::SplitRoot()
{
    BTPage  *pChild1 = 0, *pChild2 = 0, *pChild3 = 0;
    Node oi1, oi2;
    SplitPageInto3( m_Keys, m_SubPages, pChild1, pChild2, pChild3, oi1, oi2);
    clear();

    m_Keys[0]    = oi1;
    m_SubPages[0] = pChild1;
    NumberOfKeys()++;

    m_Keys[1]    = oi2;
    m_SubPages[1] = pChild2;
    NumberOfKeys()++;

    m_SubPages[2] = pChild3;
    return true;
}

//! @brief Búsqueda binaria + recursión; en caso de éxito incrementa `UseCounter`.
template <typename Traits>
bool CBTreePage<Traits>::Search(const keyType &key, obj_id_t &ObjID)
{
    btree_pos_t pos = binary_search(m_Keys, 0, m_KeyCount, key);
    if( pos >= m_KeyCount ){
        if( m_SubPages[pos] )
            return m_SubPages[pos]->Search(key, ObjID);
        else
            return false;
    }
    if( key == m_Keys[pos].key ) {
        ObjID = m_Keys[pos].ObjID;
        m_Keys[pos].UseCounter++;
        return true;
    }
    if( key < m_Keys[pos].key )
        if( m_SubPages[pos] )
            return m_SubPages[pos]->Search(key, ObjID);
    return false;
}

//! @brief Motor de recorrido único: consume el iterador materializado por
//!        la política y delega en `func(nodo, nivel, args...)`.
//! @details Si `func` devuelve `void`, se sigue hasta el final (modo
//!          `ForEach`); si devuelve convertible a `bool`, el primer
//!          `true` detiene y devuelve el nodo (modo `FirstThat`).
template <typename Traits>
template <typename Func, typename... Args>
typename CBTreePage<Traits>::Node *
CBTreePage<Traits>::Call(Func func, tree_height_t level, Args&&... args)
{
    using Result   = invoke_result_t<Func, Node&, tree_height_t, Args...>;
    using iterator = BTreeIterator<BTree<Traits>, BTreeForwardInorderPolicy>;

    iterator it(nullptr, this, level), fin(nullptr, nullptr);
    for (; it != fin; ++it)
    {
        if constexpr (is_void_v<Result>)
            func(*it, it.level(), std::forward<Args>(args)...);
        else if (func(*it, it.level(), std::forward<Args>(args)...))
            return it.getNode();
    }
    return nullptr;
}

//! @brief Llama a `Call` ignorando el valor de retorno (recorrido total).
template <typename Traits>
template <typename Func, typename... Args>
void CBTreePage<Traits>::ForEach(Func func, tree_height_t level, Args&&... args)
{
    Call(func, level, std::forward<Args>(args)...);
}

//! @brief Llama a `Call` devolviendo el primer nodo que cumple el predicado.
template <typename Traits>
template <typename Func, typename... Args>
typename CBTreePage<Traits>::Node *
CBTreePage<Traits>::FirstThat(Func func, tree_height_t level, Args&&... args)
{
    return Call(func, level, std::forward<Args>(args)...);
}

//! @brief Eliminación: cuatro casos (hoja, no-hoja, underflow, raíz).
template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Remove(const keyType &key, const ObjIDType ObjID)
{
    bt_ErrorCode error = bt_ok;
    btree_pos_t pos = binary_search(m_Keys, 0, m_KeyCount, key);
    if( pos < NumberOfKeys() && key == m_Keys[pos].key )
    {
        if( !m_SubPages[pos+1] )
        {
            ::remove(m_Keys, pos);
            NumberOfKeys()--;
            if( Underflow() ) return bt_underflow;
            return bt_ok;
        }
        {
            Node &rFirstFromRight = m_SubPages[pos+1]->GetFirstNode();
            swap(m_Keys[pos], rFirstFromRight);
            error = m_SubPages[++pos]->Remove(key, ObjID);
        }
    }
    else if( pos == NumberOfKeys() )
        error = m_SubPages[pos]->Remove(key, ObjID);
    else if( key <= m_Keys[pos].key ){
        if( m_SubPages[pos] )
            error = m_SubPages[pos]->Remove(key, ObjID);
        else
            return bt_nofound;
    }
    if( error == bt_underflow ){
        if( TreatUnderflow(pos) )
            return bt_ok;
        if( IsRoot() && NumberOfKeys() == 2 )
            return MergeRoot();
        return Merge(pos);
    }
    if( error == bt_nofound )
        return bt_nofound;
    return bt_ok;
}

//! @brief Fusiona tres páginas adyacentes equilibrando `pChild1` y `pChild2`.
template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::Merge(btree_pos_t pos)
{
    assert( m_SubPages[pos-1]->NumberOfKeys() +
            m_SubPages[ pos ]->NumberOfKeys() +
            m_SubPages[pos+1]->NumberOfKeys() ==
            3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

    vector<Node> tmpKeys;
    vector<BTPage *> tmpSubPages;

    BTPage *pChild1 = m_SubPages[pos-1],
           *pChild2 = m_SubPages[ pos ],
           *pChild3 = m_SubPages[pos+1];
    MovePage(pChild1, tmpKeys, tmpSubPages);
    tmpKeys.push_back(m_Keys[pos-1]);
    MovePage(pChild2, tmpKeys, tmpSubPages);
    tmpKeys.push_back(m_Keys[ pos ]);
    MovePage(pChild3, tmpKeys, tmpSubPages);
    pChild3->Destroy();

    btree_pos_t nKeys = pChild1->GetFreeCells();
    btree_pos_t i = 0;
    for( ; i < nKeys ; ++i ) {
        pChild1->m_Keys[i] = tmpKeys[i];
        pChild1->m_SubPages[i] = tmpSubPages[i];
        pChild1->NumberOfKeys()++;
    }
    pChild1->m_SubPages[i] = tmpSubPages[i];

    m_Keys[pos-1] = tmpKeys[i];
    m_SubPages[pos-1] = pChild1;

    ::remove(m_Keys, pos);
    ::remove(m_SubPages, pos);
    NumberOfKeys()--;

    nKeys = pChild2->GetFreeCells();
    btree_pos_t j = ++i;
    for(i = 0 ; i < nKeys ; ++i, ++j ) {
        pChild2->m_Keys[i] = tmpKeys[j];
        pChild2->m_SubPages[i] = tmpSubPages[j];
        pChild2->NumberOfKeys()++;
    }
    pChild2->m_SubPages[i] = tmpSubPages[j];
    m_SubPages[pos] = pChild2;

    if( Underflow() ) return bt_underflow;
    return bt_ok;
}

//! @brief Fusiona las tres páginas de la raíz, bajando la altura total.
template <typename Traits>
bt_ErrorCode CBTreePage<Traits>::MergeRoot()
{
    btree_pos_t pos = 1;
    assert( m_SubPages[pos-1]->NumberOfKeys() +
            m_SubPages[ pos ]->NumberOfKeys() +
            m_SubPages[pos+1]->NumberOfKeys() ==
            3*m_SubPages[ pos ]->MinNumberOfKeys() - 1);

    BTPage *pChild1 = m_SubPages[pos-1],
           *pChild2 = m_SubPages[ pos ],
           *pChild3 = m_SubPages[pos+1];
    btree_pos_t nKeys = pChild1->NumberOfKeys() + pChild2->NumberOfKeys() + pChild3->NumberOfKeys() + 2;

    vector<Node> tmpKeys;
    vector<BTPage *> tmpSubPages;

    MovePage(pChild1, tmpKeys, tmpSubPages);
    tmpKeys.push_back(m_Keys[pos-1]);
    MovePage(pChild2, tmpKeys, tmpSubPages);
    tmpKeys.push_back(m_Keys[ pos ]);
    MovePage(pChild3, tmpKeys, tmpSubPages);

    clear();
    btree_pos_t i = 0;
    for( ; i < nKeys ; ++i ){
        m_Keys[i] = tmpKeys[i];
        m_SubPages[i] = tmpSubPages[i];
        NumberOfKeys()++;
    }
    m_SubPages[i] = tmpSubPages[i];

    pChild1->Destroy();
    pChild2->Destroy();
    pChild3->Destroy();

    return bt_rootmerged;
}

//! @brief Devuelve el nodo más a la izquierda (primera hoja accesible).
template <typename Traits>
typename CBTreePage<Traits>::Node &
CBTreePage<Traits>::GetFirstNode()
{
    if( m_SubPages[0] )
        return m_SubPages[0]->GetFirstNode();
    return m_Keys[0];
}

//! @brief Inicializa los vectores con tamaño `m_MaxKeys+1`.
template <typename Traits>
void CBTreePage<Traits>::Create()
{
    Reset();
    m_Keys.resize(m_MaxKeys+1);
    m_SubPages.resize(m_MaxKeys+2, NULL);
    m_KeyCount = 0;
    m_MinKeys  = 2 * m_MaxKeys/3;
}

//! @brief Libera recursivamente todas las sub-páginas.
template <typename Traits>
void CBTreePage<Traits>::Reset()
{
    for( auto i = 0 ; i < m_KeyCount ; ++i )
        delete m_SubPages[i];
    clear();
}

//! @brief Reinicia el contador de claves sin tocar memoria.
template <typename Traits>
void CBTreePage<Traits>::clear()
{
    m_KeyCount = 0;
}

//! @brief Crea dinámicamente una página (factory usado por clientes externos).
template <typename Traits>
CBTreePage<Traits> * CreateBTreeNode (btree_pos_t maxKeys, bool unique)
{
    return new CBTreePage<Traits> (maxKeys, unique);
}

//! @brief Mueve todas las claves/hijos de `pChildPage` a los vectores aux.
template <typename Traits>
void CBTreePage<Traits>::MovePage(BTPage *pChildPage, vector<Node> &tmpKeys, vector<BTPage *> &tmpSubPages)
{
    btree_pos_t nKeys = pChildPage->GetNumberOfKeys();
    btree_pos_t i = 0;
    for( ; i < nKeys; ++i ) {
        tmpKeys.push_back(pChildPage->m_Keys[i]);
        tmpSubPages.push_back(pChildPage->m_SubPages[i]);
    }
    tmpSubPages.push_back(pChildPage->m_SubPages[i]);
    pChildPage->clear();
}

//! @brief Celdas libres en la página a la izquierda de `pos` (0 si no existe).
template <typename Traits>
btree_pos_t CBTreePage<Traits>::GetFreeCellsOnLeft(btree_pos_t pos)
{
    if( pos > 0 ) return m_SubPages[pos-1]->GetFreeCells();
    return 0;
}

//! @brief Celdas libres en la página a la derecha de `pos` (0 si no existe).
template <typename Traits>
btree_pos_t CBTreePage<Traits>::GetFreeCellsOnRight(btree_pos_t pos)
{
    if( pos < GetNumberOfKeys() ) return m_SubPages[pos+1]->GetFreeCells();
    return 0;
}

#endif
