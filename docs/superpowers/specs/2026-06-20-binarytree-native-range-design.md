# BinaryTree: Iteradores unificados con bucle nativo (`for(a: bt.func())`)

**Fecha:** 2026-06-20  
**Rama:** nueva desde `22-BinaryTree`  
**Archivos afectados:** `containers/binarytree.h`, `DemoBinaryTree.cpp`

---

## Objetivo

Reemplazar las 6 clases de iterador en `binarytree.h` por un único template parametrizado,
habilitando la sintaxis de bucle nativo C++:

```cpp
for (auto& nodo : bt.inorder())           { cout << nodo.getData(); }
for (auto& nodo : bt.reverse_inorder())   { cout << nodo.getData(); }
for (auto& nodo : bt.preorder())          { cout << nodo.getData(); }
for (auto& nodo : bt.reverse_preorder())  { cout << nodo.getData(); }
for (auto& nodo : bt.postorder())         { cout << nodo.getData(); }
for (auto& nodo : bt.reverse_postorder()) { cout << nodo.getData(); }
```

Restricciones: mantener mutex, traits, templates; evitar variables nativas.

---

## Diagnóstico del código actual

Las 6 clases existentes (`BinaryTreeForwardInorderIterator`, etc.) son **idénticas** salvo
el orden en que `construir()` llena la `deque<Node*>`. Solo esas 3 líneas cambian entre
las 6 implementaciones. Todo lo demás — `m_cola`, `avanzar()`, constructor, `operator++`,
herencia de `general_iterator` — es código duplicado.

---

## Arquitectura

### 1. Policy structs (6, uno por recorrido)

Cada struct expone un único método estático template que llena la cola de recorrido:

```
BinaryTreeForwardInorderPolicy     — LNR: izq → nodo → der
BinaryTreeBackwardInorderPolicy    — RNL: der → nodo → izq
BinaryTreeForwardPreorderPolicy    — NLR: nodo → izq → der
BinaryTreeBackwardPreorderPolicy   — NRL: nodo → der → izq
BinaryTreeForwardPostorderPolicy   — LRN: izq → der → nodo
BinaryTreeBackwardPostorderPolicy  — RLN: der → izq → nodo
```

Interfaz de cada policy:
```cpp
struct BinaryTreeForwardInorderPolicy {
    template <typename Node>
    static void construir(deque<Node*>& cola, Node* n) {
        if (!n) return;
        construir(cola, n->getChild(0));
        cola.push_back(n);
        construir(cola, n->getChild(1));
    }
};
```

### 2. `BinaryTreeIterator<Container, Policy>` — iterador unificado

Reemplaza las 6 clases. Hereda `general_iterator<Container, BinaryTreeIterator<Container, Policy>>`.
El constructor llama `Policy::construir(m_cola, pRaiz)` en lugar de `construir()` propio.

```cpp
template <typename Container, typename Policy>
class BinaryTreeIterator
    : public general_iterator<Container, BinaryTreeIterator<Container, Policy>> {
    using MySelf = BinaryTreeIterator<Container, Policy>;
    using Parent = general_iterator<Container, MySelf>;
    using Node   = typename Container::Node;
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
```

### 3. `BinaryTreeRange<Container, Policy>` — proxy de rango con lock

Adquiere `unique_lock<mutex>` al construirse y lo libera al destruirse (fin del range-for).
Garantiza que el mutex está bloqueado durante toda la iteración, igual que `ForEach()`.

```cpp
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
```

### 4. Cambios en `BinaryTree<Traits>`

**Friend declaration** (acceso a `m_pRoot` y `m_mtx` protegidos):
```cpp
template <typename, typename> friend class BinaryTreeRange;
```

**Type aliases actualizados** (mismos nombres, nuevo tipo subyacente):
```cpp
using forward_inorder_iterator    = BinaryTreeIterator<MySelf, BinaryTreeForwardInorderPolicy>;
using backward_inorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeBackwardInorderPolicy>;
using forward_preorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeForwardPreorderPolicy>;
using backward_preorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeBackwardPreorderPolicy>;
using forward_postorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeForwardPostorderPolicy>;
using backward_postorder_iterator = BinaryTreeIterator<MySelf, BinaryTreeBackwardPostorderPolicy>;
```

**6 métodos de rango nuevos** (retorno `auto` para ocultar el tipo completo):
```cpp
auto inorder()          { return BinaryTreeRange<MySelf, BinaryTreeForwardInorderPolicy   >(this, m_pRoot, m_mtx); }
auto reverse_inorder()  { return BinaryTreeRange<MySelf, BinaryTreeBackwardInorderPolicy  >(this, m_pRoot, m_mtx); }
auto preorder()         { return BinaryTreeRange<MySelf, BinaryTreeForwardPreorderPolicy  >(this, m_pRoot, m_mtx); }
auto reverse_preorder() { return BinaryTreeRange<MySelf, BinaryTreeBackwardPreorderPolicy >(this, m_pRoot, m_mtx); }
auto postorder()        { return BinaryTreeRange<MySelf, BinaryTreeForwardPostorderPolicy >(this, m_pRoot, m_mtx); }
auto reverse_postorder(){ return BinaryTreeRange<MySelf, BinaryTreeBackwardPostorderPolicy>(this, m_pRoot, m_mtx); }
```

---

## Manejo del mutex

| Método            | Adquiere mutex | Mecanismo              |
|-------------------|---------------|------------------------|
| `begin()`/`end()` | No            | Sin cambio (actual)    |
| `ForEach()`       | Sí            | `scoped_lock` interno  |
| `inorder()` etc.  | Sí            | `unique_lock` en Range |
| `toString()`      | Sí            | `scoped_lock` + begin/end directo (no usa inorder() — evita deadlock) |

`std::mutex` no es reentrante. `toString()` usa `begin()`/`end()` directamente
(dentro del mismo lock ya adquirido) y **no debe cambiarse** a `inorder()`.

---

## Compatibilidad hacia atrás

- Los 6 type aliases mantienen sus nombres exactos.
- `begin()`/`end()`, `rbegin()`/`rend()`, `pre_begin()`/`pre_end()`, etc. siguen funcionando.
- `ForEach()`, `ReverseForEach()`, `ForEachPreorder()`, etc. siguen funcionando.
- `FirstThat()` y `ReverseFirstThat()` siguen retornando `forward_inorder_iterator` y `backward_inorder_iterator`.
- `DemoBinaryTree.cpp` se actualiza para mostrar la nueva sintaxis range-for junto a las llamadas existentes.

---

## Archivos modificados

| Archivo                    | Cambio                                                              |
|----------------------------|---------------------------------------------------------------------|
| `containers/binarytree.h`  | Elimina 6 clases iterator → agrega 6 policy structs + 2 templates + friend + 6 métodos range |
| `DemoBinaryTree.cpp`       | Agrega demostración de sintaxis `for(auto& nodo : bt.inorder())`   |

---

## Rama

Crear rama `23-BinaryTreeRangeFor` desde `22-BinaryTree`.  
Todo el trabajo se hace en esa rama.
