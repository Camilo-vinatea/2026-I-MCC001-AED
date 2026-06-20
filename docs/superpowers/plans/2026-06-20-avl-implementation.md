# AVL Tree Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implementar `AVL<Traits>` en `containers/avl.h` heredando máxima funcionalidad de `BinaryTree<Traits>`, con `AVLNode` extendiendo `BinaryTree::Node` y `insert` balanceado vía 4 casos de rotación.

**Architecture:** `AVLNode<Traits>` hereda `BinaryTree<Traits>::Node` y agrega `size_t m_height = 1`. `AVL<Traits>` hereda `BinaryTree<Traits>` y overrides solo `insert()` con su propio `avl_insertar_interno()` que hace descenso BST + rebalanceo post-inserción. Todos los iteradores, recorridos, serialización y búsqueda son heredados sin cambios.

**Tech Stack:** C++17/23, `containers/binarytree.h`, `containers/basetrait.h`, `types.h`. Compilar con `make -B`.

---

## Mapa de archivos

| Archivo | Cambio |
|---|---|
| `containers/binarytree.h` | `private:` → `protected:` en línea 376 |
| `containers/avl.h` | Crear: `AVLNode<Traits>` + `AVL<Traits>` + `void DemoAVL();` |
| `DemoAVL.cpp` | Crear: verificación TDD con assertions |
| `Makefile` | Agregar `DemoAVL.cpp` a `SRCS` |
| `main.cpp` | Agregar `#include "containers/avl.h"` + llamada `DemoAVL()` |

---

### Task 1: Abrir acceso `protected` en `binarytree.h`

**Files:**
- Modify: `containers/binarytree.h:376`

- [ ] **Step 1: Cambiar `private:` a `protected:` en la sección de métodos internos**

En `containers/binarytree.h`, línea 376, cambiar:
```cpp
private:
    void insertar_interno(NodePtr& pNodo, const value_type& value, Ref ref) {
```
Por:
```cpp
protected:
    void insertar_interno(NodePtr& pNodo, const value_type& value, Ref ref) {
```
Los tres métodos (`insertar_interno`, `copiar_interno`, `escribir_interno`) están bajo esa misma etiqueta — una sola palabra cambiada expone los tres.

- [ ] **Step 2: Verificar compilación sin errores**

```bash
make -B
```
Salida esperada: compila limpio, sin errores ni warnings nuevos. Ejecutar `./main` y verificar que `DemoBinaryTree` imprime igual que antes.

- [ ] **Step 3: Commit**

```bash
git add containers/binarytree.h
git commit -m "refactor: exponer insertar_interno como protected para herencia AVL"
```

---

### Task 2: Escribir `DemoAVL.cpp` primero (TDD red)

**Files:**
- Create: `DemoAVL.cpp`
- Create: `containers/avl.h` (esqueleto mínimo)
- Modify: `Makefile`
- Modify: `main.cpp`

- [ ] **Step 1: Crear `containers/avl.h` esqueleto (solo guards e include)**

Crear `containers/avl.h` con contenido:
```cpp
#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

// AVLNode y AVL se implementan en Task 3

void DemoAVL();

#endif // __AVL_H__
```

- [ ] **Step 2: Crear `DemoAVL.cpp` con las assertions de verificación**

Crear `DemoAVL.cpp`:
```cpp
#include <iostream>
#include <cassert>
#include "containers/avl.h"

using namespace std;

void DemoAVL() {
    cout << "\n=== DemoAVL ===" << endl;

    // Insertar 1..7 en orden ascendente — caso degenerado para BST puro (altura=7)
    // AVL debe rebalancear a altura=3 (árbol perfecto para 7 nodos)
    AVL<BinaryTreeTraits<TI>> avl;
    for (int i = 1; i <= 7; ++i)
        avl.insert(i, (Ref)(i - 1));

    // --- Verificar tamaño ---
    assert(avl.size() == 7);
    cout << "OK: size == 7" << endl;

    // --- Verificar propiedad BST: inorden ascendente ---
    cout << "Inorden: ";
    TI prev = 0;
    bool sorted = true;
    for (auto& nodo : avl.inorder()) {
        cout << nodo.getData() << " ";
        if (nodo.getData() <= prev) sorted = false;
        prev = nodo.getData();
    }
    cout << endl;
    assert(sorted);
    cout << "OK: inorden ascendente" << endl;

    // --- Verificar balance: altura raiz == 3 ---
    // 7 nodos balanceados perfectamente => floor(log2(7))+1 = 3
    size_t h = avl.tree_height();
    cout << "Altura raiz: " << h << endl;
    assert(h == 3);
    cout << "OK: altura == 3 (balanceado)" << endl;

    // --- Iteradores heredados funcionan sobre AVL ---
    cout << "Preorden (NLR): ";
    for (auto& nodo : avl.preorder()) cout << nodo.getData() << " ";
    cout << endl;

    cout << "Postorden (LRN): ";
    for (auto& nodo : avl.postorder()) cout << nodo.getData() << " ";
    cout << endl;

    // --- AVL descendente ---
    AVL<BinaryTreeTraits<TI, DescendingTrait<TI>>> avlDesc;
    for (int i = 1; i <= 5; ++i)
        avlDesc.insert(i, (Ref)(i - 1));
    cout << "Descendente inorden (5..1): ";
    for (auto& nodo : avlDesc.inorder()) cout << nodo.getData() << " ";
    cout << endl;
    assert(avlDesc.size() == 5);
    cout << "OK: AVL descendente size == 5" << endl;

    cout << "=== DemoAVL OK ===" << endl;
}
```

- [ ] **Step 3: Agregar `DemoAVL.cpp` al Makefile**

En `Makefile`, cambiar:
```makefile
SRCS = main.cpp \
	   ListsDemo.cpp \
	   DemoBinaryTree.cpp
```
Por:
```makefile
SRCS = main.cpp \
	   ListsDemo.cpp \
	   DemoBinaryTree.cpp \
	   DemoAVL.cpp
```

- [ ] **Step 4: Conectar `DemoAVL()` en `main.cpp`**

En `main.cpp`, agregar include y llamada:
```cpp
#include <iostream>
#include "macros.h"
#include "lists.h"
// #include "containers/vector.h"
#include "containers/binarytree.h"
#include "containers/avl.h"        // agregar esta línea

using namespace std;

// g++ -std=c++2b main.cpp vector.cpp macros.cpp -o main
// 2011: C++11 => C++14 => C++17 => C++20 => C++23 => C++26

size_t size1() { return 0; }
unsigned char size2() { return 0; }
int main() {
    // DemoMacros();
    //DemoVector();
    // DemoConcurrentVector();
    // ListsDemo();
    DemoBinaryTree();
    DemoAVL();                     // agregar esta línea
    return 0;
}
```

- [ ] **Step 5: Verificar que falla (compile error esperado)**

```bash
make -B
```
Salida esperada: **error de compilación** — `AVL` no está definido todavía:
```
DemoAVL.cpp: error: 'AVL' was not declared in this scope
```
Esto confirma que el test existe y requiere implementación. Si compila, revisar que `avl.h` no tiene la clase `AVL` ya definida.

---

### Task 3: Implementar `AVLNode<Traits>` y `AVL<Traits>`

**Files:**
- Modify: `containers/avl.h`

- [ ] **Step 1: Reemplazar el contenido de `containers/avl.h` con implementación completa**

```cpp
#ifndef __AVL_H__
#define __AVL_H__

#include "binarytree.h"

// ============================================================
// AVLNode — extiende BinaryTree::Node con campo de altura
// ============================================================
template <typename Traits>
struct AVLNode : BinaryTree<Traits>::Node {
    using Base       = typename BinaryTree<Traits>::Node;
    using value_type = typename Traits::value_type;

    size_t m_height = 1;

    AVLNode(const value_type& data, Ref ref)
        : Base(data, ref) {}
};

// ============================================================
// AVL — árbol AVL-balanceado heredando de BinaryTree
// Override solo: insert()
// Hereda: todos los iteradores, ForEach, FirstThat, toString,
//         operator<<, operator>>, size, empty, copy/move/dtor
// ============================================================
template <typename Traits>
class AVL : public BinaryTree<Traits> {
    using Base       = BinaryTree<Traits>;
    using MyNode     = AVLNode<Traits>;
    using NodePtr    = typename Base::NodePtr;
    using value_type = typename Traits::value_type;

    // ── helpers de altura ────────────────────────────────────────
    static MyNode* avl(NodePtr n) {
        return static_cast<MyNode*>(n);  // seguro: AVL solo crea MyNode
    }
    static size_t height(NodePtr n) {
        return n ? avl(n)->m_height : 0;
    }
    static void update_height(NodePtr n) {
        if (n)
            avl(n)->m_height = 1 + max(height(n->getChild(0)),
                                        height(n->getChild(1)));
    }
    static ptrdiff_t bf(NodePtr n) {
        return static_cast<ptrdiff_t>(height(n->getChild(0)))
             - static_cast<ptrdiff_t>(height(n->getChild(1)));
    }

    // ── rotaciones ───────────────────────────────────────────────
    //
    //  rotate_right(y_ref):          rotate_left(x_ref):
    //       y              x              x              y
    //      / \           /   \           / \           /   \
    //     x   C   =>    A     y         A   y   =>    x     C
    //    / \                 / \           / \       / \
    //   A   B               B   C         B   C     A   B
    //
    void rotate_right(NodePtr& y_ref) {
        auto* y = avl(y_ref);
        auto* x = avl(y->getChild(0));
        y->setChild(0, x->getChild(1));
        x->setChild(1, y);
        update_height(y);
        update_height(x);
        y_ref = x;
    }
    void rotate_left(NodePtr& x_ref) {
        auto* x = avl(x_ref);
        auto* y = avl(x->getChild(1));
        x->setChild(1, y->getChild(0));
        y->setChild(0, x);
        update_height(x);
        update_height(y);
        x_ref = y;
    }

    // ── rebalanceo post-inserción ─────────────────────────────────
    //
    //  4 casos:
    //    LL: bf > 1, bf(izq) >= 0  → rotate_right(p)
    //    LR: bf > 1, bf(izq) < 0   → rotate_left(izq) + rotate_right(p)
    //    RR: bf < -1, bf(der) <= 0 → rotate_left(p)
    //    RL: bf < -1, bf(der) > 0  → rotate_right(der) + rotate_left(p)
    //
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

    // ── inserción BST + rebalanceo ────────────────────────────────
    void avl_insertar_interno(NodePtr& p, const value_type& v, Ref ref) {
        if (!p) {
            p = new MyNode(v, ref);
            ++this->m_size;
            return;
        }
        size_t pos = !this->m_comp(v, p->getDataRef());  // misma lógica que BST
        avl_insertar_interno(p->getChildRef(pos), v, ref);
        rebalance(p);
    }

public:
    AVL() = default;

    void insert(const value_type& v, Ref ref) {
        scoped_lock<mutex> lock(this->m_mtx);
        avl_insertar_interno(this->m_pRoot, v, ref);
    }

    // Altura del árbol (para verificación y debug)
    size_t tree_height() const {
        scoped_lock<mutex> lock(this->m_mtx);
        return height(this->m_pRoot);
    }
};

void DemoAVL();

#endif // __AVL_H__
```

- [ ] **Step 2: Compilar**

```bash
make -B
```
Salida esperada: compila sin errores. Si hay errores, verificar:
- `AVLNode<Traits>::Base` resuelve a `BinaryTree<Traits>::Node` ✓
- `NodePtr = BinaryTree<Traits>::Node*` — el `static_cast<MyNode*>` es válido porque AVL solo crea `MyNode` ✓
- `this->m_size`, `this->m_pRoot`, `this->m_comp`, `this->m_mtx` son `protected` en BinaryTree ✓

- [ ] **Step 3: Ejecutar y verificar salida**

```bash
./main
```
Salida esperada al final (sección DemoAVL):
```
=== DemoAVL ===
OK: size == 7
Inorden: 1 2 3 4 5 6 7
OK: inorden ascendente
Altura raiz: 3
OK: altura == 3 (balanceado)
Preorden (NLR): 4 2 1 3 6 5 7
Postorden (LRN): 1 3 2 5 7 6 4
Descendente inorden (5..1): 5 4 3 2 1
OK: AVL descendente size == 5
=== DemoAVL OK ===
```

El preorden `4 2 1 3 6 5 7` confirma la forma del árbol balanceado:
```
       4
      / \
     2   6
    / \ / \
   1  3 5  7
```

Si las assertions fallan (`assert` aborta con mensaje), el árbol no está balanceado — revisar `rebalance()`.

- [ ] **Step 4: Commit**

```bash
git add containers/avl.h DemoAVL.cpp Makefile main.cpp
git commit -m "feat: implementar AVL<Traits> con AVLNode heredando de BinaryTree::Node"
```

---

## Self-Review

**Cobertura del spec:**
- ✅ `binarytree.h` → `protected:` (Task 1)
- ✅ `AVLNode<Traits>` hereda `BinaryTree<Traits>::Node`, agrega `size_t m_height` (Task 3)
- ✅ `AVL<Traits>` hereda `BinaryTree<Traits>`, override solo `insert()` (Task 3)
- ✅ Descenso BST reutilizado (`!this->m_comp(v, p->getDataRef())`) (Task 3 Step 1)
- ✅ 4 casos de rotación con nested-if correcto (Task 3 Step 1)
- ✅ Verificación TDD: size, inorden, altura, descendente (Task 2 Step 2)
- ✅ Limitación conocida: copy constructor de `BinaryTree` crea `Node` base, no `AVLNode` — documentado en spec, fuera de scope

**Placeholders:** ninguno — todo el código está completo.

**Consistencia de tipos:**
- `NodePtr = typename Base::NodePtr = BinaryTree<Traits>::Node*` — usado consistentemente en `rotate_*`, `rebalance`, `avl_insertar_interno`
- `MyNode = AVLNode<Traits>` — creado en `avl_insertar_interno`, accedido via `avl()` en todas partes
- `tree_height()` usa el mismo helper privado `height(NodePtr)` — consistente
