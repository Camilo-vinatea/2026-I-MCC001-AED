# Arquitectura — EST-DATOS-MCC-001

Documentación de todas las clases, jerarquías y relaciones del repositorio.

> **Diagrama unificado interactivo:** [`docs/jerarquia_clases.md`](docs/jerarquia_clases.md) consolida en un único `classDiagram` Mermaid las 6 jerarquías siguientes (mismas vistas, en un solo grafo navegable).
> **Documentación HTML Doxygen:** ejecutar `doxygen Doxyfile` → `docs/html/inherits.html` (jerarquía gráfica SVG interactiva de toda la librería), `docs/html/classes.html` (índice de clases), `docs/html/graph_legend.html` (leyenda de los grafos).

---

## Jerarquía de contenedores

```mermaid
classDiagram
    class LinkedList~T,Comp~ {
        #Node* m_pRoot
        #Node* m_pTail
        #size_t m_size
        #Comp m_comp
        #mutex m_mtx
        +push_front(value, ref)
        +pop_front() pair
        +push_back(value, ref)
        +pop_back() pair
        +insert(value, ref)
        +operator[](index) Node&
        +size() size_t
        +toString() string
        +begin() forward_iterator
        +end() forward_iterator
        +ForEach(func, args)
        +FirstThat(func, args) forward_iterator
        -insertar_interno(pNode&, value, ref)
    }

    class DoubleLinkedList~T,Comp~ {
        +push_front(value, ref)
        +push_back(value, ref)
        +insert(value, ref)
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
        +ReverseForEach(func, args)
        +ReverseFirstThat(func, args) backward_iterator
    }

    class CircularLinkedList~Traits~ {
        +insert(value, ref)
        +getRoot() Node*
        +begin() forward_iterator
        +end() forward_iterator
    }

    class CircularDoubleLinkedList~Traits~ {
        +insert(value, ref)
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
    }

    class Vector~Traits~ {
        -Node* m_data
        -size_t m_size
        -size_t m_capacity
        -mutex m_mtx
        +push_back(value, ref)
        +size() size_t
        +ToString() string
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
        +ForEach(func, args)
        +ReverseForEach(func, args)
        +FirstThat(func, args) forward_iterator
        +ReverseFirstThat(func, args) backward_iterator
        -resize()
    }

    class Heap~Traits~ {
        -vector~Node~ m_heap
        -Comp m_comp
        +insert(value, ref)
        +extract()
        +peek_min() value_type
        +size() size_t
        +empty() bool
        -heapify_up(index)
        -heapify_down(index)
    }

    class BinaryTree~Traits~ {
        #Node* m_pRoot
        #size_t m_size
        #Comp m_comp
        #mutable mutex m_mtx
        +insert(value, ref)
        +size() size_t
        +empty() bool
        +toString() string
        +begin() forward_inorder_iterator
        +end() forward_inorder_iterator
        +rbegin() backward_inorder_iterator
        +rend() backward_inorder_iterator
        +pre_begin() forward_preorder_iterator
        +pre_end() forward_preorder_iterator
        +rpre_begin() backward_preorder_iterator
        +rpre_end() backward_preorder_iterator
        +post_begin() forward_postorder_iterator
        +post_end() forward_postorder_iterator
        +rpost_begin() backward_postorder_iterator
        +rpost_end() backward_postorder_iterator
        +inorder() BinaryTreeRange
        +reverse_inorder() BinaryTreeRange
        +preorder() BinaryTreeRange
        +reverse_preorder() BinaryTreeRange
        +postorder() BinaryTreeRange
        +reverse_postorder() BinaryTreeRange
        +ForEach(func, args)
        +ReverseForEach(func, args)
        +ForEachPreorder(func, args)
        +ForEachReversePreorder(func, args)
        +ForEachPostorder(func, args)
        +ForEachReversePostorder(func, args)
        +FirstThat(func, args) forward_inorder_iterator
        +ReverseFirstThat(func, args) backward_inorder_iterator
        #insertar_interno(pNode&, value, ref)
        #make_node(v, ref) NodePtr
        #post_insert(p&)
        -copiar_interno(dst&, src)
        -escribir_interno(os, node)
    }

    class AVL~Traits~ {
        +insert(value, ref)
        +tree_height() size_t
        +print_tree()
        #make_node(v, ref) NodePtr
        #post_insert(p&)
        -rebalance(p&)
        -rotate_left(p&)
        -rotate_right(p&)
        -height(n) size_t
        -update_height(n)
        -bf(n) ptrdiff_t
    }

    LinkedList <|-- DoubleLinkedList : hereda
    LinkedList <|-- CircularLinkedList : hereda
    DoubleLinkedList <|-- CircularDoubleLinkedList : hereda
    BinaryTree <|-- AVL : hereda
```

---

## Jerarquía de Nodos

Los nodos de `LinkedList`, `DoubleLinkedList` y `BinaryTree` están definidos como `struct Node` **anidado dentro del contenedor** — no son clases externas.

```mermaid
classDiagram
    class LinkedListNode~T~ {
        +T m_data
        +Ref m_ref
        +Node* m_pNext
        +getData() T
        +getDataRef() T&
        +getRef() Ref
        +getNext() Node*
        +getNextRef() Node*&
        +setNext(Node*)
        +virtual ~Node()
    }
    note for LinkedListNode "struct Node anidado en LinkedList"

    class DoubleLinkedListNode~T~ {
        +Node* m_pPrev
        +getNext() Node*
        +setNext(Node*)
        +getPrev() Node*
        +getPrevRef() Node*&
        +setPrev(Node*)
    }
    note for DoubleLinkedListNode "struct Node anidado en DoubleLinkedList"

    class BinaryTreeNode~Traits~ {
        +value_type m_data
        +Ref m_ref
        +Node* m_pChild[2]
        +Node* m_pParent
        +getData() value_type
        +getDataRef() value_type&
        +getChild(pos) Node*
        +getChildRef(pos) Node*&
        +getParent() Node*
        +setParent(Node*)
        +to_string() string
        +~Node()
    }
    note for BinaryTreeNode "struct Node anidado en BinaryTree — destructor en cascada"

    class AVLNode~Traits~ {
        +size_t m_height
    }
    note for AVLNode "struct fuera de AVL — hereda BinaryTree::Node, agrega m_height"

    AVLNode --|> BinaryTreeNode : hereda

    class VectorNode~Traits~ {
        +value_type m_data
        +Ref m_ref
        +GetData() value_type
        +GetDataRef() value_type&
        +GetRef() Ref
        +ToString() string
        +operator++()
        +operator+=(value)
    }

    LinkedListNode <|-- DoubleLinkedListNode : hereda
```

---

## Jerarquía de Iteradores

```mermaid
classDiagram
    class general_iterator~Container_IteratorBase~ {
        #Container* m_pContainer
        #Node* m_pNode
        +operator*() Node&
        +operator->() Node*
        +getNode() Node*
        +operator==(a, b) bool
        +operator!=(a, b) bool
    }

    class LinkedListForwardIterator~Container~ {
        +operator++() MySelf&
    }

    class DoubleLinkedListBackwardIterator~Container~ {
        +operator++() MySelf&
    }

    class CircularLinkedListForwardIterator~Container~ {
        +operator++() MySelf&
    }

    class CircularDoubleLinkedListForwardIterator~Container~ {
        +operator++() MySelf&
    }

    class CircularDoubleLinkedListBackwardIterator~Container~ {
        +operator++() MySelf&
    }

    class vector_forward_iterator~Container~ {
        +operator++() MySelf&
    }

    class vector_backward_iterator~Container~ {
        +operator++() MySelf&
    }

    class BinaryTreeIterator~Container_Policy~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }
    note for BinaryTreeIterator "Iterador unificado. Policy::construir() llena la deque\nsegún el orden: Inorder/Preorder/Postorder × Forward/Backward"

    class BinaryTreeRange~Container_Policy~ {
        -unique_lock~mutex~ m_lock
        -Iter m_begin
        -Iter m_end
        +begin() Iter
        +end() Iter
    }
    note for BinaryTreeRange "Rango iterable para range-for.\nAdquiere unique_lock durante toda la iteración.\nMovible, no copiable."

    general_iterator <|-- LinkedListForwardIterator : hereda
    general_iterator <|-- DoubleLinkedListBackwardIterator : hereda
    general_iterator <|-- CircularLinkedListForwardIterator : hereda
    general_iterator <|-- CircularDoubleLinkedListForwardIterator : hereda
    general_iterator <|-- CircularDoubleLinkedListBackwardIterator : hereda
    general_iterator <|-- vector_forward_iterator : hereda
    general_iterator <|-- vector_backward_iterator : hereda
    general_iterator <|-- BinaryTreeIterator : hereda
    BinaryTreeRange --> BinaryTreeIterator : produce
```

> `LinkedListForwardIterator` es reutilizado por `DoubleLinkedList` como `forward_iterator`.
> `BinaryTreeIterator` reemplaza las 6 clases anteriores — la Policy en el parámetro de template selecciona el orden de recorrido.

---

## Traits (Vector, Heap y BinaryTree)

`LinkedList` y `DoubleLinkedList` se parametrizan directamente con `template<T, Comp = less<T>>`. `Vector`, `Heap` y **`BinaryTree`** usan el patrón Traits.

```mermaid
classDiagram
    class BaseContainerTrait~_T_Node~ {
        +value_type
        +Node
    }

    class AscendingTrait~T~ {
        +Comp = less~T~
    }

    class DescendingTrait~T~ {
        +Comp = greater~T~
    }

    class VectorTraits~T~ {
    }

    class AscendingHeapTrait~T~ {
    }

    class DescendingHeapTrait~T~ {
    }

    class BinaryTreeTraits~T_CompTrait~ {
        +value_type = T
        +Comp = CompTrait::Comp
    }
    note for BinaryTreeTraits "CompTrait = AscendingTrait~T~ por defecto\nUso: BinaryTreeTraits~int~ → ascendente\n      BinaryTreeTraits~int,DescendingTrait~int~~ → descendente"

    BaseContainerTrait <|-- VectorTraits : hereda
    BaseContainerTrait <|-- AscendingHeapTrait : hereda
    BaseContainerTrait <|-- DescendingHeapTrait : hereda
    AscendingTrait <|-- AscendingHeapTrait : hereda
    DescendingTrait <|-- DescendingHeapTrait : hereda
    AscendingTrait <|-- BinaryTreeTraits : hereda por defecto
```

---

## Asociación: Contenedor ↔ Nodo anidado ↔ Iterador

```mermaid
classDiagram
    direction LR

    class LinkedList~T_Comp~ {
        struct Node nested
        Comp = less~T~ por defecto
    }
    class LinkedListNode~T~ {
        m_data, m_ref, m_pNext
    }
    class LinkedListForwardIterator~Container~

    LinkedList *-- LinkedListNode : contiene
    LinkedList --> LinkedListForwardIterator : begin/end

    class DoubleLinkedList~T_Comp~ {
        struct Node extends LinkedList_Node
        sobreescribe push/insert
    }
    class DoubleLinkedListNode~T~ {
        m_pPrev, getNext sombrea padre
    }
    class DoubleLinkedListBackwardIterator~Container~

    DoubleLinkedList *-- DoubleLinkedListNode : contiene
    DoubleLinkedListNode --|> LinkedListNode : hereda
    DoubleLinkedList --> LinkedListForwardIterator : begin/end
    DoubleLinkedList --> DoubleLinkedListBackwardIterator : rbegin/rend

    class BinaryTree~Traits~ {
        struct Node nested
        Traits = BinaryTreeTraits~T,CompTrait~
    }
    class BinaryTreeNode~Traits~ {
        value_type m_data, Ref m_ref
        Node* m_pChild[2], Node* m_pParent
    }
    class BTIter~Container_Policy~
    class BTRange~Container_Policy~

    BinaryTree *-- BinaryTreeNode : contiene
    BinaryTree --> BTIter : begin/end/rbegin/rend
    BinaryTree --> BTRange : inorder/preorder/postorder

    class AVL~Traits~ {
        hereda BinaryTree~Traits~
        override insert()
        agrega tree_height(), print_tree()
    }
    class AVLNode~Traits~ {
        hereda BinaryTree_Node
        size_t m_height
    }

    AVL --|> BinaryTree : hereda
    AVL *-- AVLNode : crea
    AVLNode --|> BinaryTreeNode : hereda
```

---

## Flujo de inserción ordenada (insert)

```mermaid
sequenceDiagram
    participant Cliente
    participant LinkedList
    participant insertar_interno

    Cliente->>LinkedList: insert(value, ref)
    LinkedList->>insertar_interno: insertar_interno(m_pRoot, value, ref)

    loop Recorrido recursivo hasta posición correcta
        insertar_interno->>insertar_interno: ¿comp(value, pNode->data)?
        alt Sí: insertar aquí
            insertar_interno->>insertar_interno: pNode = new Node(value, ref, pNode)
            insertar_interno->>insertar_interno: actualizar m_pTail si pNode == m_pRoot
        else No: avanzar
            insertar_interno->>insertar_interno: insertar_interno(pNode->getNextRef(), value, ref)
        end
    end
```

---

## Flujo de inserción en lista circular (CLL / CDLL)

```mermaid
sequenceDiagram
    participant Cliente
    participant CircularList
    participant BaseInsert as LinkedList::insertar_interno

    Cliente->>CircularList: insert(value, ref)
    CircularList->>CircularList: scoped_lock(m_mtx)
    CircularList->>CircularList: romper link circular (tail->next = nullptr)
    CircularList->>BaseInsert: insertar_interno(m_pRoot, value, ref)
    BaseInsert-->>CircularList: inserción ordenada completa
    CircularList->>CircularList: restaurar link circular (tail->next = root)
```

---

## Mapa de archivos

```mermaid
graph TD
    main["main.cpp<br/>Entry point"]
    lists["lists.h<br/>Declaraciones de demos"]
    types["types.h<br/>TI, TD, TS, Ref, XT"]
    macros["macros.h / macros.cpp<br/>SUMA, MULT, MAX, CUADRADO, GetVar"]
    foreach["foreach.h<br/>ForEach, FirstThat genéricos"]
    basetrait["containers/basetrait.h<br/>BaseContainerTrait (Vector, Heap)"]
    geniter["containers/general_iterator.h<br/>general_iterator CRTP"]
    vector["containers/vector.h<br/>Vector + VectorNode + Traits"]
    ll["containers/linkedlist.h<br/>LinkedList + struct Node anidado"]
    dll["containers/doublelinkedlist.h<br/>DoubleLinkedList + struct Node anidado"]
    cll["containers/circularlinkedlist.h<br/>CircularLinkedList"]
    cdll["containers/circulardoublelinkedlist.h<br/>CircularDoubleLinkedList"]
    heap["containers/heap.h<br/>Heap + HeapNode + Traits"]
    bt["containers/binarytree.h<br/>BinaryTree + BinaryTreeIterator + BinaryTreeRange<br/>BinaryTreeTraits + 6 Policies"]
    avl["containers/avl.h<br/>AVL + AVLNode"]

    DemoLL["DoubleLinkedListDemo.cpp"]
    DemoCLL["CircularLinkedListDemo.cpp"]
    DemoCDLL["CircularDoubleLinkedListDemo.cpp"]
    DemoHeap["HeapDemo.cpp"]
    DemoBT["DemoBinaryTree.cpp"]
    DemoAVL["DemoAVL.cpp"]

    main --> lists
    main --> macros
    main --> bt
    main --> avl
    DemoLL --> dll
    DemoCLL --> cll
    DemoCDLL --> cdll
    DemoHeap --> heap
    DemoBT --> bt
    DemoAVL --> avl

    dll --> ll
    cll --> ll
    cdll --> dll
    avl --> bt

    ll --> geniter
    ll --> types
    ll --> foreach
    vector --> geniter
    vector --> basetrait
    vector --> types
    heap --> basetrait
    heap --> types
    heap --> foreach
    bt --> geniter
    bt --> types
    bt --> foreach
    bt --> basetrait
```

---

## Flujo de inserción en BST (BinaryTree)

```mermaid
sequenceDiagram
    participant Cliente
    participant BinaryTree
    participant insertar_interno

    Cliente->>BinaryTree: insert(value, ref)
    BinaryTree->>BinaryTree: scoped_lock(m_mtx)
    BinaryTree->>insertar_interno: insertar_interno(m_pRoot, value, ref)

    loop Descenso recursivo BST
        insertar_interno->>insertar_interno: ¿pNode == nullptr?
        alt Sí: posición encontrada
            insertar_interno->>insertar_interno: pNode = make_node(value, ref)
            insertar_interno->>insertar_interno: ++m_size
        else No: elegir hijo
            insertar_interno->>insertar_interno: pos = !comp(value, pNode->data)
            insertar_interno->>insertar_interno: insertar_interno(pNode->getChildRef(pos), value, ref)
            insertar_interno->>insertar_interno: post_insert(pNode)
        end
    end
```

---

## Iteradores de BinaryTree — Órdenes de recorrido

Todos usan la clase unificada `BinaryTreeIterator<Container, Policy>`. La `Policy` selecciona el orden:

| Policy | Orden | Método de acceso directo | Método range-for |
|---|---|---|---|
| `BinaryTreeForwardInorderPolicy` | LNR (izq → nodo → der) | `begin()` / `end()` | `inorder()` |
| `BinaryTreeBackwardInorderPolicy` | RNL (der → nodo → izq) | `rbegin()` / `rend()` | `reverse_inorder()` |
| `BinaryTreeForwardPreorderPolicy` | NLR (nodo → izq → der) | `pre_begin()` / `pre_end()` | `preorder()` |
| `BinaryTreeBackwardPreorderPolicy` | NRL (nodo → der → izq) | `rpre_begin()` / `rpre_end()` | `reverse_preorder()` |
| `BinaryTreeForwardPostorderPolicy` | LRN (izq → der → nodo) | `post_begin()` / `post_end()` | `postorder()` |
| `BinaryTreeBackwardPostorderPolicy` | RLN (der → izq → nodo) | `rpost_begin()` / `rpost_end()` | `reverse_postorder()` |

> `deque<Node*>` pre-construida en constructor (`Policy::construir()` recursivo). `operator++` solo avanza la deque. Coste: O(n) tiempo y espacio.
>
> Los métodos `inorder()` / `preorder()` / etc. devuelven `BinaryTreeRange` — adquieren `unique_lock<mutex>` durante toda la iteración, habilitando `for(auto& n : bt.inorder())`.
>
> **Advertencia:** no llamar `.inorder()` dentro de un método que ya tenga `scoped_lock` sobre el mismo mutex — `BinaryTree::mutex` no es reentrante. Ver `toString()` que usa iterador directo sin lock.

---

## AVL — árbol balanceado

`AVL<Traits>` hereda todo de `BinaryTree<Traits>`. Usa el patrón **Template Method**: `BinaryTree::insertar_interno` deja dos hooks virtuales que AVL sobreescribe — `make_node()` crea `AVLNode` en vez de `Node`, y `post_insert()` llama `rebalance()` al volver de cada nivel recursivo. No duplica la lógica de descenso BST.

### Nodo AVL

`AVLNode<Traits>` extiende `BinaryTree<Traits>::Node` con un campo `size_t m_height`. El árbol AVL solo crea nodos de tipo `AVLNode` y usa `static_cast` interno para acceder a `m_height`.

### Factor de balance y rotaciones

| Factor de balance `bf = h(izq) - h(der)` | Caso | Acción |
|---|---|---|
| `bf > 1` y `bf(izq) >= 0` | LL | `rotate_right(p)` |
| `bf > 1` y `bf(izq) < 0` | LR | `rotate_left(izq)` → `rotate_right(p)` |
| `bf < -1` y `bf(der) <= 0` | RR | `rotate_left(p)` |
| `bf < -1` y `bf(der) > 0` | RL | `rotate_right(der)` → `rotate_left(p)` |

### API exclusiva de AVL

| Método | Descripción |
|--------|-------------|
| `insert(value, ref)` | Llama `BinaryTree::insertar_interno` — hooks `make_node`/`post_insert` inyectan `AVLNode` y `rebalance()` |
| `tree_height()` | Altura del árbol (O(1) — lee `m_height` de la raíz) |
| `print_tree()` | Árbol girado 90°: rama derecha arriba, izquierda abajo, muestra `dato (h=N)` |

### Herencia completa desde BinaryTree

AVL reutiliza sin cambios: todos los iteradores, `ForEach` / variantes, `FirstThat`, `toString`, `operator<<`, `operator>>`, `size()`, `empty()`, constructores de copia/movimiento, destructor.

---

## Heap — operaciones y complejidad

```mermaid
flowchart TD
    insert["insert(v, ref)\nO(log n)"]
    extract["extract()\nO(log n)"]
    peek["peek_min()\nO(1)"]

    insert --> HU["heapify_up\nburbujea hacia la raíz"]
    extract --> HD["heapify_down\nbaja el último a raíz"]
    peek --> R["retorna m_heap[0]"]
```

| Operación | Complejidad | Descripción |
|-----------|-------------|-------------|
| `insert` | O(log n) | Inserta al final, sube hasta posición correcta |
| `extract` | O(log n) | Extrae raíz, sube último elemento, baja |
| `peek_min` | O(1) | Accede `m_heap[0]` sin modificar |

---

## Operadores de stream

| Clase | `operator<<` | `operator>>` |
|-------|-------------|--------------|
| `VectorNode` | imprime `(data, ref)` | — |
| `Vector` | imprime `[n0, n1, ...]` | — |
| `LinkedList::Node` | imprime `(data, ref)` | — |
| `LinkedList` | llama `toString()` | `push_back` desde stream |
| `DoubleLinkedList` | hereda `LinkedList::operator<<` | hereda `LinkedList::operator>>` |
| `BinaryTree::Node` | imprime `Nodo(dato: X, ref: Y)` | lee `dato ref` por línea |
| `BinaryTree` | escribe `n` + nodos en preorden (`dato ref\n`) | lee conteo + `dato ref` → `insert()` |

---

## Concurrencia

Todos los contenedores usan `std::mutex m_mtx` (en `protected` de `LinkedList`, heredado por `DoubleLinkedList` / `mutable` en `BinaryTree`).

| Operación | Mecanismo |
|-----------|-----------|
| `push_front / push_back` | `scoped_lock<mutex>` |
| `pop_front / pop_back` | `scoped_lock<mutex>` |
| `insert` (LinkedList) | sin lock en entrada — `insertar_interno` privado sin lock |
| `insert` (DoubleLinkedList) | `scoped_lock<mutex>` en override |
| `toString` (LinkedList) | sin lock — uso interno |
| `ForEach` (LinkedList) | `unique_lock<mutex>` |
| `ForEach` (DoubleLinkedList) | `unique_lock<mutex>` |
| `BinaryTree::insert` | `scoped_lock<mutex>` |
| `BinaryTree::ForEach` / variantes | `scoped_lock<mutex>` |
| `BinaryTree::FirstThat` / `ReverseFirstThat` | `scoped_lock<mutex>` |
| `BinaryTree::toString` | `scoped_lock<mutex>` |
| `BinaryTree::operator<<` | `scoped_lock<mutex>` |
| `BinaryTree` copy/move constructors | `scoped_lock<mutex>` sobre `other.m_mtx` |
| `BinaryTree` destructor | `scoped_lock<mutex>` |
| `BinaryTree::inorder()` / variantes range-for | `unique_lock<mutex>` — mantenido durante toda la iteración |
| `AVL::insert` | `scoped_lock<mutex>` (override de BinaryTree::insert) |
| `AVL::tree_height` | `scoped_lock<mutex>` |
| `AVL::print_tree` | `scoped_lock<mutex>` |

> `insertar_interno`, `make_node`, `post_insert`, `copiar_interno`, `escribir_interno`, `rebalance`, rotaciones — no adquieren lock propio, siempre llamados bajo lock del método público.

---

## Patrones de diseño utilizados

| Patrón | Dónde |
|--------|-------|
| **CRTP** (Curiously Recurring Template Pattern) | `general_iterator<Container, IteratorBase>` |
| **Nodo anidado** (nested Node) | `struct Node` dentro de `LinkedList`, `DoubleLinkedList`, `BinaryTree` |
| **Traits** | `Vector`, `Heap`, `BinaryTree` (via `BinaryTreeTraits<T, CompTrait>`) |
| **Policy** | `BinaryTreeIterator<Container, Policy>` — 6 policies seleccionan el orden de recorrido |
| **RAII** | `scoped_lock<mutex>` en todas las operaciones mutantes; `unique_lock` en `BinaryTreeRange` |
| **Move semantics** | constructores y operadores de movimiento en `LinkedList`, `BinaryTree`; `BinaryTreeRange` movible |
| **Variadic templates** | `ForEach`, `FirstThat` con `Args&&...` |
| **Perfect forwarding** | `std::forward<Args>(args)...` en ForEach/FirstThat |
| **Template Method** | `BinaryTree::insertar_interno` define el esqueleto BST; hooks virtuales `make_node` y `post_insert` permiten que `AVL` cambie tipo de nodo y añada rebalanceo sin duplicar lógica |
| **Herencia de template** | `AVL<Traits>` extiende `BinaryTree<Traits>`; `AVLNode` extiende `Node` |
