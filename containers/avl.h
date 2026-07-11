//! @file avl.h
//! @brief Árbol AVL (BST auto-balanceado) basado en `BinaryTree` (Template Method).
//! @details Hereda de `BinaryTree<Traits>` y sobreescribe:
//!          - `make_node` → construye nodos `AVLNode` con altura.
//!          - `post_insert` → invoca `rebalance` tras insertar.
//!          Implementa rotaciones simples (LL, RR) y dobles (LR, RL).
//! @author Equipo MCC

#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

/* ===================== AVLNode ====================================== */
//! @struct AVLNode
//! @brief Nodo AVL: añade `m_height` al nodo del BST.
//! @tparam Traits Trait del BST.
template <typename Traits>
struct AVLNode : BinaryTree<Traits>::Node {
    using Base       = typename BinaryTree<Traits>::Node;
    using value_type = typename Traits::value_type;

    //! @brief Altura del subárbol cuya raíz es este nodo.
    size_t m_height = 1;

    //! @brief Constructor.
    AVLNode(const value_type& data, Ref ref)
        : Base(data, ref) {}
};

/* =========================== AVL ==================================== */
//! @class AVL
//! @brief Árbol AVL: BST que se reequilibra tras cada inserción.
//! @tparam Traits Trait del BST (define `value_type` y `Comp`).
template <typename Traits>
class AVL : public BinaryTree<Traits> {
    using Base       = BinaryTree<Traits>;
    using MyNode     = AVLNode<Traits>;
    using NodePtr    = typename Base::NodePtr;
    using value_type = typename Traits::value_type;

    /* ----------------- helpers de altura y balanceo ---------------- */
    //! @brief Cast seguro a `AVLNode` (AVL solo crea nodos `AVLNode`).
    static MyNode* avl(NodePtr n) {
        return static_cast<MyNode*>(n);
    }
    //! @brief Altura del subárbol; 0 para `nullptr`.
    static size_t height(NodePtr n) {
        return n ? avl(n)->m_height : 0;
    }
    //! @brief Recalcula la altura de `n` a partir de sus hijos.
    static void update_height(NodePtr n) {
        if (n)
            avl(n)->m_height = 1 + max(height(n->getChild(0)),
                                       height(n->getChild(1)));
    }
    //! @brief Factor de balanceo = `altura(izq) - altura(der)`.
    static ptrdiff_t bf(NodePtr n) {
        return static_cast<ptrdiff_t>(height(n->getChild(0)))
             - static_cast<ptrdiff_t>(height(n->getChild(1)));
    }

    /* ----------------------- rotaciones ----------------------------- */
    //! @brief Rotación a la derecha sobre `y_ref` (padre sube como hijo izq).
    //! @image html rotate_right.png
    void rotate_right(NodePtr& y_ref) {
        auto* y = avl(y_ref);
        auto* x = avl(y->getChild(0));
        y->setChild(0, x->getChild(1));
        x->setChild(1, y);
        update_height(y);
        update_height(x);
        y_ref = x;
    }
    //! @brief Rotación a la izquierda sobre `x_ref` (padre sube como hijo der).
    void rotate_left(NodePtr& x_ref) {
        auto* x = avl(x_ref);
        auto* y = avl(x->getChild(1));
        x->setChild(1, y->getChild(0));
        y->setChild(0, x);
        update_height(x);
        update_height(y);
        x_ref = y;
    }

    /* ---------------- rebalanceo post-inserción --------------------- */
    //! @brief Aplica rotaciones para dejar el árbol balanceado.
    //! @details 4 casos:
    //!   - LL: `bf > 1, bf(izq) >= 0` → `rotate_right(p)`
    //!   - LR: `bf > 1, bf(izq) <  0` → `rotate_left(izq)` + `rotate_right(p)`
    //!   - RR: `bf < -1, bf(der) <= 0` → `rotate_left(p)`
    //!   - RL: `bf < -1, bf(der) >  0` → `rotate_right(der)` + `rotate_left(p)`
    void rebalance(NodePtr& p) {
        update_height(p);
        ptrdiff_t b = bf(p);
        if (b > 1) {
            if (bf(p->getChild(0)) < 0)
                rotate_left(p->getChildRef(0));  // LR → convierte a LL
            rotate_right(p);                      // LL
        } else if (b < -1) {
            if (bf(p->getChild(1)) > 0)
                rotate_right(p->getChildRef(1)); // RL → convierte a RR
            rotate_left(p);                       // RR
        }
    }

protected:
    //! @brief Construye un `AVLNode` en vez del `Node` del padre.
    NodePtr make_node(const value_type& v, Ref ref) override {
        return new MyNode(v, ref);
    }
    //! @brief Hook de post-inserción: dispara el rebalanceo.
    void post_insert(NodePtr& p) override { rebalance(p); }

public:
    //! @brief Constructor por defecto.
    AVL() = default;

    //! @brief Inserta `(v, ref)` con `scoped_lock` del BST padre.
    void insert(const value_type& v, Ref ref) {
        scoped_lock<mutex> lock(this->m_mtx);
        this->insertar_interno(this->m_pRoot, v, ref);
    }

    //! @brief Altura del árbol entero (para verificación).
    size_t tree_height() const {
        scoped_lock<mutex> lock(this->m_mtx);
        return height(this->m_pRoot);
    }

    //! @brief Imprime el árbol girado 90° (derecha arriba, izquierda abajo).
    void print_tree() const {
        scoped_lock<mutex> lock(this->m_mtx);
        if (!this->m_pRoot) { cout << "  (vacio)\n"; return; }
        print_interno(this->m_pRoot, 0);
    }

private:
    //! @brief Impresión recursiva con sangrado de 4 espacios por nivel.
    static void print_interno(NodePtr n, int nivel) {
        if (!n) return;
        print_interno(n->getChild(1), nivel + 1);
        cout << string(nivel * 4, ' ')
             << n->getData() << "  (h=" << avl(n)->m_height << ")\n";
        print_interno(n->getChild(0), nivel + 1);
    }
};

//! @brief Demo del AVL: inserciones, balanceo automático, comparador descendente.
void DemoAVL();

#endif // __AVL_H__
