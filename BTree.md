# BTree — Clases y Relaciones

## Diagrama de clases

```mermaid
classDiagram
    class BTreeTraits~K V~ {
        +KeyType : K
        +ObjIDType : V
    }

    class BTree~Traits~ {
        #m_Root : CBTreePage
        #m_Height : tree_height_t
        #m_Order : tree_order_t
        #m_NumKeys : size_t
        #m_Unique : bool
        #m_Mutex : shared_mutex
        +BTree(order, unique)
        +Insert(key, ObjID) bool
        +Remove(key, ObjID) bool
        +Search(key) ObjIDType
        +size() size_t
        +height() tree_height_t
        +GetOrder() tree_order_t
        +Print(os) void
        +ForEach(lpfn, args) void
        +ForEachInternal(func, args) void
        +FirstThat(func, args) Node*
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
    }

    class BTreeIterator~Container Policy~ {
        -m_cola : deque~pair~Node*, tree_height_t~~
        -m_nivel : tree_height_t
        +BTreeIterator(pC, pRaiz, nivel = 0)
        +BTreeIterator(pC, nullptr)
        +operator++() MySelf&
        +level() tree_height_t
        -avanzar() void
    }

    class BTreeForwardInorderPolicy {
        <<policy>>
        +construir(cola, p, nivel)$ void
    }

    class BTreeBackwardInorderPolicy {
        <<policy>>
        +construir(cola, p, nivel)$ void
    }

    class tagNode~keyType ObjIDType~ {
        +key : keyType
        +ObjID : ObjIDType
        +UseCounter : long
        +tagNode(key, ObjID)
        +GetUseCounter() long
        +operator keyType()
    }

    class bt_ErrorCode {
        <<enumeration>>
        bt_ok
        bt_overflow
        bt_underflow
        bt_duplicate
        bt_nofound
        bt_rootmerged
    }

    class CBTreePage~keyType ObjIDType~ {
        #m_MinKeys : int
        #m_MaxKeys : int
        #m_MaxKeysForChilds : int
        #m_Unique : bool
        #m_isRoot : bool
        #m_Keys : vector~tagNode~
        #m_SubPages : vector~CBTreePage*~
        #m_KeyCount : int
        +CBTreePage(maxKeys, unique)
        +Insert(key, ObjID) bt_ErrorCode
        +Remove(key, ObjID) bt_ErrorCode
        +Search(key, ObjID) bool
        +Print(os) void
        +ForEach(func, level, args) void
        +FirstThat(func, level, args) Node*
        -Call(func, level, args) Node*
        #Overflow() bool
        #Underflow() bool
        #IsFull() bool
        #IsRoot() bool
        #Redistribute1(pos) bool
        #Redistribute2(pos) bool
        #RedistributeR2L(pos) void
        #RedistributeL2R(pos) void
        #TreatUnderflow(pos) bool
        #Merge(pos) bt_ErrorCode
        #MergeRoot() bt_ErrorCode
        #SplitChild(pos) void
        #GetFirstNode() Node
        -SplitRoot() bool
        -SplitPageInto3(...) void
        -MovePage(...) void
    }

    BTreeTraits <.. BTree : Traits
    BTree *-- CBTreePage : m_Root
    CBTreePage *-- tagNode : m_Keys
    CBTreePage o-- CBTreePage : m_SubPages
    CBTreePage ..> bt_ErrorCode : retorna
    BTree ..> bt_ErrorCode : usa
    BTree ..> BTreeIterator : crea (begin/end/rbegin/rend)
    BTreeIterator ..> BTreeForwardInorderPolicy : Policy
    BTreeIterator ..> BTreeBackwardInorderPolicy : Policy
    BTreeIterator --> tagNode : itera sobre Node*
    BTreeForwardInorderPolicy ..> CBTreePage : friend, recorre m_Keys/m_SubPages
    CBTreePage ..> BTreeIterator : Call itera con él
```

---

## Iteradores forward/backward (BTreePage.h)

`BTreeIterator<Container, Policy>` y las policies viven en `BTreePage.h`
(se movieron desde `BTree.h` para que `CBTreePage::Call` pueda usarlos).
El iterador hereda de `general_iterator` y recorre el árbol en orden usando
una cola (`deque<pair<Node*, tree_height_t>>`) llenada de una sola vez por
la `Policy` en el constructor. Cada entrada guarda el nodo y su nivel de
profundidad, expuesto vía `level()`.

- **BTreeForwardInorderPolicy::construir** — recorre la página directamente
  (recursión in-order sobre `m_Keys`/`m_SubPages`, es `friend` de `CBTreePage`),
  empujando cada `(Node*, nivel)` a la cola en orden ascendente. No usa
  `ForEach`: eso crearía dependencia circular con `Call`, que ahora consume
  el iterador.
- **BTreeBackwardInorderPolicy::construir** — reusa `BTreeForwardInorderPolicy::construir`
  y luego hace `std::reverse` sobre la cola. No existe un `ForEachReverse` propio:
  el recorrido inverso se logra invirtiendo el resultado forward.
- `operator++` (`avanzar()`) hace `pop_front()` de la cola; al vaciarse, `m_pNode = nullptr` (fin).

`BTree` expone los alias `forward_iterator` / `backward_iterator` y los métodos
`begin()/end()` (con `scoped_lock`) y `rbegin()/rend()`, habilitando range-for.
Además, `BTree::ForEach(lpfn, args...)` delega en el `::ForEach` externo de
`foreach.h` construyendo los iteradores directamente (no vía `begin()/end()`,
que re-tomarían el mutex); a diferencia de `::ForEach(bt.begin(), bt.end(), ...)`,
el mutex queda tomado durante todo el recorrido.

```cpp
for (auto& node : miBTree)        { /* forward, in-order */ }
for (auto it = bt.rbegin(); it != bt.rend(); ++it) { /* backward */ }
bt.ForEach([](Node& n){ ... });   // ::ForEach externo, mutex sostenido
```

```mermaid
flowchart TD
    A["BTree::begin()"] --> B["BTreeIterator(this, &m_Root)"]
    B --> C["Policy::construir(m_cola, pRaiz, nivel=0)"]
    C --> D["recursión in-order directa sobre\nm_Keys / m_SubPages (friend)"]
    D --> E{Policy == Backward?}
    E -->|sí| F["std::reverse(m_cola)"]
    E -->|no| G["avanzar(): pop_front → m_pNode, m_nivel"]
    F --> G
```

---

## Flujo de Insert

```mermaid
flowchart TD
    A["BTree::Insert(key, ObjID)"] --> B["m_Root.Insert(key, ObjID)"]
    B --> C["binary_search → pos"]
    C --> D{duplicado & unique?}
    D -->|sí| E[bt_duplicate]
    D -->|no| F{¿es hoja?}
    F -->|sí| G["insert_at(m_Keys, Node, pos)\nm_KeyCount++"]
    F -->|no| H["m_SubPages[pos]->Insert(key, ObjID)"]
    G --> I{Overflow?}
    H --> J{error == bt_overflow?}
    J -->|no| K[bt_ok]
    J -->|sí| L[Redistribute1?]
    L -->|sí| M[Redistribución]
    L -->|no| N[SplitChild: 2 páginas → 3]
    M --> O{¿Overflow padre?}
    N --> O
    O -->|sí| P[bt_overflow]
    O -->|no| K
    I -->|no| K
    I -->|sí| P
    E --> Q["BTree: return false"]
    K --> R["BTree: m_NumKeys++\nreturn true"]
    P --> S["BTree: SplitRoot()\nm_Height++\nreturn true"]
```

---

## Flujo de Remove

```mermaid
flowchart TD
    A["BTree::Remove(key, ObjID)"] --> B["m_Root.Remove(key, ObjID)"]
    B --> C["binary_search → pos"]
    C --> D{¿key encontrado?}
    D -->|no existe| E[bt_nofound]
    D -->|sí, en hoja| F["remove(m_Keys, pos)\nm_KeyCount--"]
    D -->|sí, en interno| G["swap con GetFirstNode(sucesor)\nSubPages[pos+1]->Remove(key)"]
    D -->|no aquí| H["SubPages[pos]->Remove(key)"]
    F --> I{Underflow?}
    G --> I
    H --> I
    I -->|no| J[bt_ok]
    I -->|sí| K["TreatUnderflow(pos)\n= Redistribute1 OR Redistribute2"]
    K -->|exitoso| J
    K -->|falla| L{IsRoot && keys==2?}
    L -->|sí| M["MergeRoot()\nbt_rootmerged"]
    L -->|no| N["Merge(pos):\n3 hijos → 2 hijos"]
    N --> O{Underflow?}
    O -->|sí| P[bt_underflow]
    O -->|no| J
    E --> Q["BTree: return false"]
    M --> R["BTree: m_Height--\nreturn true"]
    J --> S["BTree: m_NumKeys--\nreturn true"]
    P --> T["BTree: return false"]
```

---

## Call: motor único de ForEach/FirstThat (BTreePage.h)

`CBTreePage::Call` es privado y hace el recorrido real. `ForEach` y `FirstThat`
son wrappers públicos de una línea que delegan en él. Ya no es recursivo:
construye un `BTreeIterator` forward sobre `this` (la recursión in-order vive
en la policy, que materializa la cola completa) y hace un loop lineal sobre él,
recuperando el nivel de cada nodo con `it.level()`.
El modo (void = visita todos / bool = busca primero que cumpla) se decide en
tiempo de compilación con `if constexpr (is_void_v<Result>)`, según el tipo de
retorno de `Func`. No hay branching en runtime por nodo.

Costo: materializar la cola es O(N) en memoria por recorrido, frente al O(altura)
de la versión recursiva anterior; se acepta como precio de unificar el recorrido
sobre el iterador.

```mermaid
flowchart TD
    A["ForEach(func, level, args)"] --> C["Call(func, level, args)"]
    B["FirstThat(func, level, args)"] --> C
    C --> D["BTreeIterator forward(nullptr, this, level)\n(policy llena la cola in-order con niveles)"]
    D --> E["for it != fin; ++it"]
    E --> F{"Result == void?"}
    F -->|sí| G["func(*it, it.level(), args)"]
    F -->|no| H{"func(*it, it.level(), args)?"}
    H -->|sí| I["return it.getNode()"]
    H -->|no| E
    G --> E
    E -->|cola vacía| J["return nullptr"]
```

---

## Funciones libres (BTreePage.h)

```mermaid
classDiagram
    class binary_search~Container ObjType~ {
        <<function>>
        +binary_search(container, first, last, object) int
    }
    class insert_at~Container ObjType~ {
        <<function>>
        +insert_at(container, object, pos) void
    }
    class remove~Container~ {
        <<function>>
        +remove(container, pos) void
    }

    CBTreePage ..> binary_search : búsqueda de posición
    CBTreePage ..> insert_at : inserción ordenada
    CBTreePage ..> remove : eliminación por pos
```
