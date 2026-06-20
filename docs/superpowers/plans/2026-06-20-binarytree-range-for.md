# BinaryTree Range-For Iteradores Unificados — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reemplazar las 6 clases de iterador en `binarytree.h` por un único `BinaryTreeIterator<Container, Policy>` parametrizado, habilitando la sintaxis `for(auto& nodo : bt.inorder())`.

**Architecture:** Seis Policy structs (uno por recorrido) con método estático `construir()` template definen el orden de la deque. Un único `BinaryTreeIterator<Container, Policy>` delega a `Policy::construir()`. Un `BinaryTreeRange<Container, Policy>` agrupa begin/end y adquiere `unique_lock<mutex>` durante todo el loop.

**Tech Stack:** C++23 (`-std=c++2b`), `<mutex>`, `<deque>`, templates, CRTP via `general_iterator`. Build: `make` (g++ con `-pthread`).

**Spec:** `docs/superpowers/specs/2026-06-20-binarytree-native-range-design.md`

---

## File Map

| Archivo | Acción | Responsabilidad |
|---------|--------|-----------------|
| `containers/binarytree.h` | Modificar | Eliminar 6 clases iterator → agregar 6 policy structs + `BinaryTreeIterator` + `BinaryTreeRange` + friend + aliases + 6 métodos range |
| `DemoBinaryTree.cpp` | Modificar | Reemplazar 6 llamadas `bt.ForEach*()` con bucles `for(auto& nodo : bt.inorder())` |

---

## Task 1: Crear rama de trabajo

**Files:**
- (ninguno — solo git)

- [ ] **Step 1: Crear rama desde `22-BinaryTree`**

```bash
git checkout -b 23-BinaryTreeRangeFor
```

Expected: `Switched to a new branch '23-BinaryTreeRangeFor'`

- [ ] **Step 2: Verificar estado limpio**

```bash
git status
```

Expected: `nothing to commit, working tree clean`

---

## Task 2: Escribir el uso objetivo en DemoBinaryTree.cpp (TDD — compile falla primero)

Esto define el contrato que el código debe satisfacer antes de implementar.

**Files:**
- Modify: `DemoBinaryTree.cpp`

- [ ] **Step 1: Reemplazar las 6 llamadas `ForEach` con bucles nativos**

En `DemoBinaryTree.cpp`, reemplazar este bloque (líneas 27–49):

```cpp
    cout << "\nInorden Adelante  (LNR): ";
    bt.ForEach(imprimirNodo);
    cout << endl;

    cout << "Inorden Reverso   (RNL): ";
    bt.ReverseForEach(imprimirNodo);
    cout << endl;

    cout << "Preorden Adelante  (NLR): ";
    bt.ForEachPreorder(imprimirNodo);
    cout << endl;

    cout << "Preorden Reverso   (NRL): ";
    bt.ForEachReversePreorder(imprimirNodo);
    cout << endl;

    cout << "Postorden Adelante (LRN): ";
    bt.ForEachPostorder(imprimirNodo);
    cout << endl;

    cout << "Postorden Reverso  (RLN): ";
    bt.ForEachReversePostorder(imprimirNodo);
    cout << endl;
```

Por este bloque equivalente con bucle nativo:

```cpp
    cout << "\nInorden Adelante  (LNR): ";
    for (auto& nodo : bt.inorder())           cout << nodo.getData() << " ";
    cout << endl;

    cout << "Inorden Reverso   (RNL): ";
    for (auto& nodo : bt.reverse_inorder())   cout << nodo.getData() << " ";
    cout << endl;

    cout << "Preorden Adelante  (NLR): ";
    for (auto& nodo : bt.preorder())          cout << nodo.getData() << " ";
    cout << endl;

    cout << "Preorden Reverso   (NRL): ";
    for (auto& nodo : bt.reverse_preorder())  cout << nodo.getData() << " ";
    cout << endl;

    cout << "Postorden Adelante (LRN): ";
    for (auto& nodo : bt.postorder())         cout << nodo.getData() << " ";
    cout << endl;

    cout << "Postorden Reverso  (RLN): ";
    for (auto& nodo : bt.reverse_postorder()) cout << nodo.getData() << " ";
    cout << endl;
```

- [ ] **Step 2: Verificar que el lambda `imprimirNodo` permanece** (lo usan bt2, bt3, btDesc más abajo)

El lambda en línea 23–25 debe quedar intacto:
```cpp
    auto imprimirNodo = [](auto& nodo) {
        cout << nodo.getData() << " ";
    };
```

- [ ] **Step 3: Intentar compilar — debe fallar**

```bash
make 2>&1 | head -20
```

Expected (error de compilación confirmando que el contrato aún no existe):
```
DemoBinaryTree.cpp: error: 'inorder' is not a member of 'BinaryTree<BinaryTreeTraits<int>>'
```

---

## Task 3: Agregar los 6 policy structs a `binarytree.h`

Los policy structs definen el orden de llenado de la deque. Cada uno tiene un método estático `construir()` con template en `Node` — sin conocer el tipo `Container`.

**Files:**
- Modify: `containers/binarytree.h`

- [ ] **Step 1: Insertar los 6 policy structs ANTES del comentario `// --- Inorden forward`**

Insertar inmediatamente después de la línea `using namespace std;` (línea 16) y antes del bloque de comentario `// ============================================================` que introduce los iteradores:

```cpp
// ============================================================
// BuildPolicy structs — definen el orden de recorrido
// Cada uno: template<Node> static void construir(deque<Node*>&, Node*)
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

```

- [ ] **Step 2: Compilar — debe seguir fallando solo por `inorder` faltante**

```bash
make 2>&1 | head -20
```

Expected: mismo error que antes (`'inorder' is not a member`). Los policy structs no rompen nada.

---

## Task 4: Agregar `BinaryTreeIterator` y `BinaryTreeRange` a `binarytree.h`

**Files:**
- Modify: `containers/binarytree.h`

- [ ] **Step 1: Insertar `BinaryTreeIterator<Container, Policy>` DESPUÉS de las 6 clases iterator existentes y ANTES del comentario `// Traits helper`**

Insertar entre `BinaryTreeBackwardPostorderIterator` (que termina en `};`) y el bloque `// ============================================================\n// Traits helper`:

```cpp
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

```

- [ ] **Step 2: Compilar — debe seguir fallando solo por `inorder` faltante**

```bash
make 2>&1 | head -20
```

Expected: mismo error (`'inorder' is not a member`). Las nuevas clases no rompen nada.

---

## Task 5: Actualizar `BinaryTree<Traits>` y eliminar las 6 clases antiguas

**Files:**
- Modify: `containers/binarytree.h`

- [ ] **Step 1: Agregar `friend` declaration dentro de `BinaryTree` justo después de `using MySelf = BinaryTree<Traits>;`**

Localizar:
```cpp
    using MySelf     = BinaryTree<Traits>;
```

Agregar inmediatamente después:
```cpp
    template <typename, typename> friend class BinaryTreeRange;
```

- [ ] **Step 2: Reemplazar los 6 type aliases existentes**

Localizar y reemplazar:
```cpp
    using forward_inorder_iterator    = BinaryTreeForwardInorderIterator<MySelf>;
    using backward_inorder_iterator   = BinaryTreeBackwardInorderIterator<MySelf>;
    using forward_preorder_iterator   = BinaryTreeForwardPreorderIterator<MySelf>;
    using backward_preorder_iterator  = BinaryTreeBackwardPreorderIterator<MySelf>;
    using forward_postorder_iterator  = BinaryTreeForwardPostorderIterator<MySelf>;
    using backward_postorder_iterator = BinaryTreeBackwardPostorderIterator<MySelf>;
```

Por:
```cpp
    using forward_inorder_iterator    = BinaryTreeIterator<MySelf, BinaryTreeForwardInorderPolicy>;
    using backward_inorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeBackwardInorderPolicy>;
    using forward_preorder_iterator   = BinaryTreeIterator<MySelf, BinaryTreeForwardPreorderPolicy>;
    using backward_preorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeBackwardPreorderPolicy>;
    using forward_postorder_iterator  = BinaryTreeIterator<MySelf, BinaryTreeForwardPostorderPolicy>;
    using backward_postorder_iterator = BinaryTreeIterator<MySelf, BinaryTreeBackwardPostorderPolicy>;
```

- [ ] **Step 3: Agregar los 6 métodos de rango en `BinaryTree`**

Localizar el bloque `// --- Iteradores postorden ---` que contiene:
```cpp
    // --- Iteradores postorden ---
    forward_postorder_iterator  post_begin()   { return {this, m_pRoot}; }
    forward_postorder_iterator  post_end()     { return {this, nullptr}; }
    backward_postorder_iterator rpost_begin()  { return {this, m_pRoot}; }
    backward_postorder_iterator rpost_end()    { return {this, nullptr}; }
```

Agregar inmediatamente DESPUÉS de ese bloque (antes de `// --- Recorridos completos ---`):

```cpp
    // --- Rangos nativos: for(auto& n : bt.inorder()) ---
    auto inorder()          { return BinaryTreeRange<MySelf, BinaryTreeForwardInorderPolicy   >(this, m_pRoot, m_mtx); }
    auto reverse_inorder()  { return BinaryTreeRange<MySelf, BinaryTreeBackwardInorderPolicy  >(this, m_pRoot, m_mtx); }
    auto preorder()         { return BinaryTreeRange<MySelf, BinaryTreeForwardPreorderPolicy  >(this, m_pRoot, m_mtx); }
    auto reverse_preorder() { return BinaryTreeRange<MySelf, BinaryTreeBackwardPreorderPolicy >(this, m_pRoot, m_mtx); }
    auto postorder()        { return BinaryTreeRange<MySelf, BinaryTreeForwardPostorderPolicy >(this, m_pRoot, m_mtx); }
    auto reverse_postorder(){ return BinaryTreeRange<MySelf, BinaryTreeBackwardPostorderPolicy>(this, m_pRoot, m_mtx); }

```

- [ ] **Step 4: Eliminar las 6 clases de iterador antiguas**

Eliminar el bloque completo entre `// --- Inorden forward: LNR` y el cierre `};` de `BinaryTreeBackwardPostorderIterator` (líneas 26–192 originales, ahora desplazadas). Es el bloque que contiene las 6 clases:

```
BinaryTreeForwardInorderIterator     (con sus construir/avanzar/constructor/operator++)
BinaryTreeBackwardInorderIterator    (ídem)
BinaryTreeForwardPreorderIterator    (ídem)
BinaryTreeBackwardPreorderIterator   (ídem)
BinaryTreeForwardPostorderIterator   (ídem)
BinaryTreeBackwardPostorderIterator  (ídem)
```

También eliminar el comentario que las precede:
```cpp
// ============================================================
// Iteradores de recorrido
// Todos siguen el mismo patrón:
//   - Constructor (Container*, raíz) construye la deque y llama advance()
//   - Constructor (Container*, nullptr) produce el iterador end() (deque vacía)
//   - operator++ avanza la deque
// ============================================================
```

- [ ] **Step 5: Compilar — debe compilar sin errores**

```bash
make 2>&1
```

Expected:
```
g++ -std=c++2b -Wall -g -pthread -c main.cpp -o main.o
g++ -std=c++2b -Wall -g -pthread -c ListsDemo.cpp -o ListsDemo.o
g++ -std=c++2b -Wall -g -pthread -c DemoBinaryTree.cpp -o DemoBinaryTree.o
g++ -pthread main.o ListsDemo.o DemoBinaryTree.o -o main
```

Sin warnings ni errores. Si aparece algún warning, investigar antes de continuar.

---

## Task 6: Ejecutar, verificar salida y hacer commit final

**Files:**
- (ninguno — verificación y git)

- [ ] **Step 1: Ejecutar el programa**

```bash
./main
```

- [ ] **Step 2: Verificar la sección de BinaryTree**

La salida de `DemoBinaryTree` debe incluir exactamente:

```
=== DemoBinaryTree ===
Tamanio: 7
toString (inorden): [Nodo(dato: 20, ref: 3),Nodo(dato: 30, ref: 1),Nodo(dato: 40, ref: 4),Nodo(dato: 50, ref: 0),Nodo(dato: 60, ref: 5),Nodo(dato: 70, ref: 2),Nodo(dato: 80, ref: 6)]

Inorden Adelante  (LNR): 20 30 40 50 60 70 80
Inorden Reverso   (RNL): 80 70 60 50 40 30 20
Preorden Adelante  (NLR): 50 30 20 40 70 60 80
Preorden Reverso   (NRL): 50 70 80 60 30 40 20
Postorden Adelante (LRN): 20 40 30 60 80 70 50
Postorden Reverso  (RLN): 80 60 70 40 20 30 50

Primero con dato > 35: 40

Copia (inorden): 20 30 40 50 60 70 80
Movimiento (inorden, origen vacio=1): 20 30 40 50 60 70 80

Escrito en binarytree.txt

Leido desde disco (inorden): 20 30 40 50 60 70 80

BST descendente (inorden): 80 70 50 40 30 20
```

Verificar que:
- Los 6 recorridos producen el mismo resultado que antes de la refactorización
- Copy/Move constructors siguen funcionando (usan `ForEach` — sin cambio)
- E/S en disco sigue funcionando
- BST descendente sigue funcionando

- [ ] **Step 3: Commit**

```bash
git add containers/binarytree.h DemoBinaryTree.cpp
git commit -m "$(cat <<'EOF'
feat: unificar 6 iteradores BinaryTree en BinaryTreeIterator<Policy> con range-for

Reemplaza BinaryTreeForward/BackwardInorder/Preorder/PostorderIterator
por BinaryTreeIterator<Container,Policy> parametrizado por 6 BuildPolicy
structs. Agrega BinaryTreeRange<Container,Policy> con unique_lock<mutex>
para habilitar for(auto& nodo : bt.inorder()).

Co-Authored-By: Claude Sonnet 4.6 <noreply@anthropic.com>
EOF
)"
```

---

## Notas críticas para el implementador

### Orden de elementos en `binarytree.h` después de todos los cambios

```
1. #ifndef / #include / using namespace std
2. [NUEVO] 6 BuildPolicy structs
3. [NUEVO] BinaryTreeIterator<Container, Policy>
4. [NUEVO] BinaryTreeRange<Container, Policy>
5. BinaryTreeTraits (sin cambio)
6. BinaryTree<Traits> (con friend + aliases actualizados + 6 métodos range)
7. void DemoBinaryTree();
8. #endif
```

### Qué NO tocar

- `general_iterator.h` — sin cambios
- `foreach.h` — sin cambios
- Los métodos `ForEach()`, `ReverseForEach()`, etc. en `BinaryTree` — quedan intactos
- Los métodos `begin()`, `end()`, `rbegin()`, `rend()`, `pre_begin()`, etc. — quedan intactos
- `toString()` usa `begin()`/`end()` directamente dentro de `scoped_lock` — **no cambiar a `inorder()`** (causaría deadlock: `std::mutex` no es reentrante)
- Las llamadas a `bt2.ForEach(imprimirNodo)`, `bt3.ForEach(imprimirNodo)`, `btDesc.ForEach(imprimirNodo)` en `DemoBinaryTree.cpp` — quedan intactas

### Unique_lock vs scoped_lock

`BinaryTreeRange` usa `unique_lock<mutex>` (movible) en lugar de `scoped_lock` (no movible). Esto permite que el compilador materialice el prvalue de `bt.inorder()` directamente en el range-for sin copia.
