//! @file binarytree.h
//! @brief Árbol binario de búsqueda (BST) genérico basado en políticas.
//! @details Define:
//!          - 6 políticas de recorrido (in/pre/postorder, sentido forward/
//!            backward) que llenan una `deque<Node*>` mediante recursión.
//!          - `BinaryTreeIterator<Container, Policy>`: iterador único
//!            basado en CRTP que materializa la cola al construirse.
//!          - `BinaryTreeRange<...>`: adaptador para `range-for`
//!            (`for(auto& n : bt.inorder())`) con `unique_lock<mutex>`
//!            durante toda la iteración.
//!          - `BinaryTreeTraits<T, CompTrait>`: trait que combina el valor
//!            y el comparador (`AscendingTrait` o `DescendingTrait`).
//!          - `BinaryTree<Traits>`: BST con nodo anidado, inserciones,
//!            recorridos, `FirstThat`, persistencia en streams,
//!            copia/movimiento, concurrencia con `scoped_lock<mutex>`.
//! @author Equipo MCC

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

/* =================== Políticas de recorrido ========================== */
/* Cada una rellena un `deque<Node*>` con el orden deseado.             */

//! @struct BinaryTreeForwardInorderPolicy
//! @brief Recorrido inorden hacia adelante (Izq → Nodo → Der).
struct BinaryTreeForwardInorderPolicy {
    //! @brief Llamada recursiva; añade `n` a `cola` en orden LNR.
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(0));
        cola.push_back(n);
        construir(cola, n->getChild(1));
    }
};

//! @struct BinaryTreeBackwardInorderPolicy
//! @brief Recorrido inorden inverso (Der → Nodo → Izq).
struct BinaryTreeBackwardInorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(1));
        cola.push_back(n);
        construir(cola, n->getChild(0));
    }
};

//! @struct BinaryTreeForwardPreorderPolicy
//! @brief Recorrido preorden hacia adelante (Nodo → Izq → Der).
struct BinaryTreeForwardPreorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        cola.push_back(n);
        construir(cola, n->getChild(0));
        construir(cola, n->getChild(1));
    }
};

//! @struct BinaryTreeBackwardPreorderPolicy
//! @brief Recorrido preorden inverso (Nodo → Der → Izq).
struct BinaryTreeBackwardPreorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        cola.push_back(n);
        construir(cola, n->getChild(1));
        construir(cola, n->getChild(0));
    }
};

//! @struct BinaryTreeForwardPostorderPolicy
//! @brief Recorrido postorden hacia adelante (Izq → Der → Nodo).
struct BinaryTreeForwardPostorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(0));
        construir(cola, n->getChild(1));
        cola.push_back(n);
    }
};

//! @struct BinaryTreeBackwardPostorderPolicy
//! @brief Recorrido postorden inverso (Der → Izq → Nodo).
struct BinaryTreeBackwardPostorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(1));
        construir(cola, n->getChild(0));
        cola.push_back(n);
    }
};

/* ================ Iterador basado en políticas ======================= */
//! @class BinaryTreeIterator
//! @brief Iterador uniforme que materializa la cola al construirse.
//! @tparam Container Contenedor dueño del nodo.
//! @tparam Policy    Política que decide el orden de recorrido.
template <typename Container, typename Policy>
class BinaryTreeIterator
    : public general_iterator<Container, BinaryTreeIterator<Container, Policy>>
{
    using MySelf = BinaryTreeIterator<Container, Policy>;
    using Parent = general_iterator<Container, MySelf>;
public:
    using Node = typename Container::Node;
private:
    deque<Node*> m_cola;              //!< Cola de nodos en orden.

    //! @brief Avanza la cabeza de la cola o la fija a `nullptr` al final.
    void avanzar() {
        if (!m_cola.empty()) { this->m_pNode = m_cola.front(); m_cola.pop_front(); }
        else                  { this->m_pNode = nullptr; }
    }
public:
    //! @brief Construye el iterador recorriendo `pRaiz` con la política.
    BinaryTreeIterator(Container* pC, Node* pRaiz)
        : Parent(pC, nullptr) { Policy::construir(m_cola, pRaiz); avanzar(); }

    //! @brief Incremento prefijo.
    MySelf& operator++() { avanzar(); return *this; }
};

/* ==================== Rango para range-for =========================== */
//! @class BinaryTreeRange
//! @brief Adapta un par begin/end a rango iterable para `range-for`.
//! @details Adquiere `unique_lock<mutex>` durante toda la iteración.
//! @warning No usar dentro de métodos que ya tengan `scoped_lock`: el
//!          mutex no es reentrante (ver `toString()` en `BinaryTree`).
//! @tparam Container Contenedor dueño.
//! @tparam Policy    Política de recorrido.
template <typename Container, typename Policy>
class BinaryTreeRange {
    using Iter = BinaryTreeIterator<Container, Policy>;
    unique_lock<mutex> m_lock;   //!< Lock durante toda la vida del rango.
    Iter               m_begin;
    Iter               m_end;
public:
    //! @brief Constructor; adquiere el mutex del contenedor.
    BinaryTreeRange(Container* pC, typename Container::Node* pRoot, mutex& mtx)
        : m_lock(mtx)
        , m_begin(pC, pRoot)
        , m_end  (pC, nullptr)
    {}
    //! @brief Constructor de movimiento (el lock se transfiere).
    BinaryTreeRange(BinaryTreeRange&&)            = default;
    //! @brief Prohibido copiar.
    BinaryTreeRange(const BinaryTreeRange&)       = delete;
    //! @brief Prohibido asignar por copia.
    BinaryTreeRange& operator=(BinaryTreeRange&&) = delete;

    //! @brief Iterador de inicio.
    Iter begin() { return m_begin; }
    //! @brief Iterador final.
    Iter end()   { return m_end;   }
};

/* ======================== Trait de usuario =========================== */
//! @struct BinaryTreeTraits
//! @brief Combina el tipo de valor y el comparador (asc/desc).
template <typename T, typename CompTrait = AscendingTrait<T>>
struct BinaryTreeTraits : CompTrait {
    using value_type = T;   //!< Tipo de dato almacenado.
};

/* ========================== BinaryTree =============================== */
//! @class BinaryTree
//! @brief Árbol binario de búsqueda con nodo anidado dentro del contenedor.
//! @tparam Traits Debe exponer `value_type` y `Comp`.
template <typename Traits>
class BinaryTree {
public:
    using value_type = typename Traits::value_type;  //!< Valor almacenado.
    using Comp       = typename Traits::Comp;        //!< Comparador.
    struct Node;                                      //!< Forward del nodo.
    using NodePtr    = Node*;                         //!< Alias del puntero.
    using MySelf     = BinaryTree<Traits>;            //!< Alias propio.

    //! @brief Permite que `BinaryTreeRange` acceda a métodos privados.
    template <typename, typename> friend class BinaryTreeRange;

    //! @struct Node
    //! @brief Nodo del BST: dato, `Ref`, dos hijos y puntero al padre.
    struct Node {
        using value_type = typename Traits::value_type;
        value_type m_data;             //!< Dato almacenado.
        Ref        m_ref;              //!< Identificador asociado.
        Node*      m_pChild[2];        //!< Hijos [0]=izq, [1]=der.
        Node*      m_pParent;          //!< Padre (no usado en todas las ops).

        //! @brief Constructor con dato, ref y dos hijos opcionales.
        Node(const value_type& data, const Ref& ref,
             Node* izq = nullptr, Node* der = nullptr)
            : m_data(data), m_ref(ref), m_pParent(nullptr)
        { m_pChild[0] = izq; m_pChild[1] = der; }

        //! @brief Constructor de copia (recursivo, clona subárbol).
        Node(const Node& other)
            : m_data(other.m_data), m_ref(other.m_ref), m_pParent(nullptr)
        {
            m_pChild[0] = other.m_pChild[0] ? new Node(*other.m_pChild[0]) : nullptr;
            m_pChild[1] = other.m_pChild[1] ? new Node(*other.m_pChild[1]) : nullptr;
        }

        //! @brief Constructor de movimiento.
        Node(Node&& other) noexcept
            : m_data(move(other.m_data)),
              m_ref(exchange(other.m_ref, Ref{})),
              m_pParent(nullptr)
        {
            m_pChild[0] = exchange(other.m_pChild[0], nullptr);
            m_pChild[1] = exchange(other.m_pChild[1], nullptr);
        }

        //! @brief Destructor recursivo: borra los hijos (cascada).
        ~Node() { delete m_pChild[0]; delete m_pChild[1]; }

        value_type  getData()    const { return m_data; }
        value_type& getDataRef()       { return m_data; }
        void        setData(const value_type& d) { m_data = d; }
        Ref    getRef()     const { return m_ref; }
        Ref&   getRefRef()        { return m_ref; }
        void   setRef(Ref r)      { m_ref = r; }

        //! @brief Devuelve el hijo en `pos` (0=izq, 1=der).
        Node*  getChild(size_t pos)     const { return m_pChild[pos]; }
        //! @brief Referencia mutable al hijo `pos`.
        Node*& getChildRef(size_t pos)        { return m_pChild[pos]; }
        //! @brief Establece el hijo en `pos`.
        void   setChild(size_t pos, Node* p)  { m_pChild[pos] = p; }

        Node*  getParent()               const { return m_pParent; }
        void   setParent(Node* p)              { m_pParent = p; }

        //! @brief Representación textual del nodo.
        string to_string() const {
            ostringstream ss;
            ss << "Nodo(dato: " << m_data << ", ref: " << m_ref << ")";
            return ss.str();
        }

        //! @brief Operador `<<` por stream.
        friend ostream& operator<<(ostream& os, const Node& n) {
            return os << n.to_string();
        }

        //! @brief Operador `>>` desde stream (formato `dato ref` por línea).
        friend istream& operator>>(istream& is, Node& n) {
            string linea;
            if (getline(is, linea)) {
                istringstream ss(linea);
                ss >> n.m_data >> n.m_ref;
            }
            return is;
        }
    };

    //! @name Alias de iteradores pre/post/in orden, sentidos forward/backward.
    ///@{
    using forward_inorder_iterator    = BinaryTreeIterator<MySelf, BinaryTreeForwardInorderPolicy>;
    using backward_inorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeBackwardInorderPolicy>;
    using forward_preorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeForwardPreorderPolicy>;
    using backward_preorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeBackwardPreorderPolicy>;
    using forward_postorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeForwardPostorderPolicy>;
    using backward_postorder_iterator = BinaryTreeIterator<MySelf, BinaryTreeBackwardPostorderPolicy>;
    ///@}

protected:
    NodePtr        m_pRoot = nullptr;   //!< Raíz del BST.
    size_t         m_size  = 0;          //!< Número de nodos.
    Comp           m_comp;               //!< Comparador.
    mutable mutex  m_mtx;                //!< Mutex para concurrencia.

public:
    //! @brief Constructor por defecto.
    BinaryTree() = default;

    //! @brief Constructor de copia (copia recursiva).
    BinaryTree(const BinaryTree& other) {
        scoped_lock<mutex> lock(other.m_mtx);
        copiar_interno(m_pRoot, other.m_pRoot);
        m_size = other.m_size;
    }

    //! @brief Constructor de movimiento.
    BinaryTree(BinaryTree&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        m_pRoot = exchange(other.m_pRoot, nullptr);
        m_size  = exchange(other.m_size,  size_t{0});
    }

    //! @brief Destructor: borra la raíz (cascada).
    ~BinaryTree() {
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;
        m_pRoot = nullptr;
    }

    //! @brief Inserta `(value, ref)` ordenadamente. Thread-safe.
    void insert(const value_type& value, Ref ref) {
        scoped_lock<mutex> lock(m_mtx);
        insertar_interno(m_pRoot, value, ref);
    }

    //! @brief Número de nodos.
    size_t size()  const { return m_size; }
    //! @brief `true` si el árbol está vacío.
    bool   empty() const { return m_size == 0; }

    //! @name Iteradores inorder
    ///@{
    forward_inorder_iterator  begin()   { return {this, m_pRoot}; }
    forward_inorder_iterator  end()     { return {this, nullptr}; }
    backward_inorder_iterator rbegin()  { return {this, m_pRoot}; }
    backward_inorder_iterator rend()    { return {this, nullptr}; }
    ///@}

    //! @name Iteradores preorder
    ///@{
    forward_preorder_iterator  pre_begin()  { return {this, m_pRoot}; }
    forward_preorder_iterator  pre_end()    { return {this, nullptr}; }
    backward_preorder_iterator rpre_begin() { return {this, m_pRoot}; }
    backward_preorder_iterator rpre_end()   { return {this, nullptr}; }
    ///@}

    //! @name Iteradores postorder
    ///@{
    forward_postorder_iterator  post_begin()  { return {this, m_pRoot}; }
    forward_postorder_iterator  post_end()    { return {this, nullptr}; }
    backward_postorder_iterator rpost_begin() { return {this, m_pRoot}; }
    backward_postorder_iterator rpost_end()   { return {this, nullptr}; }
    ///@}

    //! @name Rangos para `range-for` (`for(auto& n : bt.inorder())`)
    ///@{
    auto inorder()          { return BinaryTreeRange<MySelf, BinaryTreeForwardInorderPolicy   >(this, m_pRoot, m_mtx); }
    auto reverse_inorder()  { return BinaryTreeRange<MySelf, BinaryTreeBackwardInorderPolicy  >(this, m_pRoot, m_mtx); }
    auto preorder()         { return BinaryTreeRange<MySelf, BinaryTreeForwardPreorderPolicy  >(this, m_pRoot, m_mtx); }
    auto reverse_preorder() { return BinaryTreeRange<MySelf, BinaryTreeBackwardPreorderPolicy >(this, m_pRoot, m_mtx); }
    auto postorder()        { return BinaryTreeRange<MySelf, BinaryTreeForwardPostorderPolicy >(this, m_pRoot, m_mtx); }
    auto reverse_postorder(){ return BinaryTreeRange<MySelf, BinaryTreeBackwardPostorderPolicy>(this, m_pRoot, m_mtx); }
    ///@}

    //! @name Recorridos completos con función variádica
    ///@{
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
    ///@}

    //! @name Búsqueda condicional
    ///@{
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
    ///@}

    //! @brief Convierte el árbol en texto in-order `[n1,n2,...]`.
    //! @warning Adquiere el mutex; evita anidar dentro de un `scoped_lock`.
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

    //! @brief Operador `<<`: imprime tamaño y nodos en preorder.
    friend ostream& operator<<(ostream& os, MySelf& bt) {
        scoped_lock<mutex> lock(bt.m_mtx);
        os << bt.m_size << "\n";
        bt.escribir_interno(os, bt.m_pRoot);
        return os;
    }

    //! @brief Operador `>>`: lee tamaño + líneas `dato ref`.
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

protected:
    //! @brief Hook para crear el nodo (permite que `AVL` fabrique su propio tipo).
    virtual NodePtr make_node(const value_type& v, Ref ref) { return new Node(v, ref); }
    //! @brief Hook invocado tras cada inserción (usado por `AVL::rebalance`).
    virtual void post_insert(NodePtr&) {}

    //! @brief Inserción recursiva; usa `make_node` y `post_insert`.
    void insertar_interno(NodePtr& pNodo, const value_type& value, Ref ref) {
        if (!pNodo) {
            pNodo = make_node(value, ref);
            ++m_size;
            return;
        }
        size_t pos = !m_comp(value, pNodo->getDataRef());
        insertar_interno(pNodo->getChildRef(pos), value, ref);
        post_insert(pNodo);
    }

    //! @brief Copia recursiva (usada por el constructor de copia).
    void copiar_interno(NodePtr& dst, const NodePtr src) {
        if (!src) { dst = nullptr; return; }
        dst = new Node(src->getData(), src->getRef());
        copiar_interno(dst->getChildRef(0), src->getChild(0));
        copiar_interno(dst->getChildRef(1), src->getChild(1));
    }

    //! @brief Escribe nodos en preorder (`data ref` por línea).
    void escribir_interno(ostream& os, const Node* nodo) const {
        if (!nodo) return;
        os << nodo->getData() << " " << nodo->getRef() << "\n";
        escribir_interno(os, nodo->getChild(0));
        escribir_interno(os, nodo->getChild(1));
    }
};

//! @brief Demo del BST con inserciones, recorridos, copia/movimiento y E/S.
void DemoBinaryTree();

#endif // __BINARY_TREE_H__
