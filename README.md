# Arquitectura — EST-DATOS-MCC-001

Documentación de todas las clases, jerarquías y relaciones del repositorio.

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

    class BinaryTree~T,Comp~ {
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
        +post_begin() forward_postorder_iterator
        +ForEach(func, args)
        +ReverseForEach(func, args)
        +ForEachPreorder(func, args)
        +ForEachReversePreorder(func, args)
        +ForEachPostorder(func, args)
        +ForEachReversePostorder(func, args)
        +FirstThat(func, args) forward_inorder_iterator
        +ReverseFirstThat(func, args) backward_inorder_iterator
        -insertar_interno(pNode&, value, ref)
        -copiar_interno(dst&, src)
        -escribir_interno(os, node)
    }

    LinkedList <|-- DoubleLinkedList : hereda
    LinkedList <|-- CircularLinkedList : hereda
    DoubleLinkedList <|-- CircularDoubleLinkedList : hereda
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

    class BinaryTreeNode~T~ {
        +T m_data
        +Ref m_ref
        +Node* m_pChild[2]
        +Node* m_pParent
        +getData() T
        +getDataRef() T&
        +getChild(pos) Node*
        +getChildRef(pos) Node*&
        +to_string() string
        +~Node()
    }
    note for BinaryTreeNode "struct Node anidado en BinaryTree — destructor en cascada"

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

    class BinaryTreeForwardInorderIterator~Container~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }
    class BinaryTreeBackwardInorderIterator~Container~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }
    class BinaryTreeForwardPreorderIterator~Container~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }
    class BinaryTreeBackwardPreorderIterator~Container~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }
    class BinaryTreeForwardPostorderIterator~Container~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }
    class BinaryTreeBackwardPostorderIterator~Container~ {
        -deque~Node*~ m_cola
        +operator++() MySelf&
    }

    general_iterator <|-- LinkedListForwardIterator : hereda
    general_iterator <|-- DoubleLinkedListBackwardIterator : hereda
    general_iterator <|-- CircularLinkedListForwardIterator : hereda
    general_iterator <|-- CircularDoubleLinkedListForwardIterator : hereda
    general_iterator <|-- CircularDoubleLinkedListBackwardIterator : hereda
    general_iterator <|-- vector_forward_iterator : hereda
    general_iterator <|-- vector_backward_iterator : hereda
    general_iterator <|-- BinaryTreeForwardInorderIterator : hereda
    general_iterator <|-- BinaryTreeBackwardInorderIterator : hereda
    general_iterator <|-- BinaryTreeForwardPreorderIterator : hereda
    general_iterator <|-- BinaryTreeBackwardPreorderIterator : hereda
    general_iterator <|-- BinaryTreeForwardPostorderIterator : hereda
    general_iterator <|-- BinaryTreeBackwardPostorderIterator : hereda
```

> `LinkedListForwardIterator` es reutilizado por `DoubleLinkedList` como `forward_iterator`.

---

## Traits (solo Vector y Heap)

`LinkedList`, `DoubleLinkedList` y `BinaryTree` ya **no** usan traits — se parametrizan directamente con `template<T, Comp = less<T>>` y definen su nodo internamente. `Vector` y `Heap` mantienen el patrón Traits.

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

    BaseContainerTrait <|-- VectorTraits : hereda
    BaseContainerTrait <|-- AscendingHeapTrait : hereda
    BaseContainerTrait <|-- DescendingHeapTrait : hereda
    AscendingTrait <|-- AscendingHeapTrait : hereda
    DescendingTrait <|-- DescendingHeapTrait : hereda
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

    class BinaryTree~T_Comp~ {
        struct Node nested
        Comp = less~T~ por defecto
    }
    class BinaryTreeNode~T~ {
        m_data, m_ref, m_pChild[2]
    }
    class BTFwdInorder~Container~
    class BTBwdInorder~Container~

    BinaryTree *-- BinaryTreeNode : contiene
    BinaryTree --> BTFwdInorder : begin/end
    BinaryTree --> BTBwdInorder : rbegin/rend
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
    bt["containers/binarytree.h<br/>BinaryTree + struct Node anidado"]

    DemoLL["DoubleLinkedListDemo.cpp"]
    DemoCLL["CircularLinkedListDemo.cpp"]
    DemoCDLL["CircularDoubleLinkedListDemo.cpp"]
    DemoHeap["HeapDemo.cpp"]
    DemoBT["DemoBinaryTree.cpp"]

    main --> lists
    main --> macros
    main --> bt
    DemoLL --> dll
    DemoCLL --> cll
    DemoCDLL --> cdll
    DemoHeap --> heap
    DemoBT --> bt

    dll --> ll
    cll --> ll
    cdll --> dll

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
            insertar_interno->>insertar_interno: pNode = new Node(value, ref)
            insertar_interno->>insertar_interno: ++m_size
        else No: elegir hijo
            insertar_interno->>insertar_interno: pos = !comp(value, pNode->data)
            insertar_interno->>insertar_interno: insertar_interno(pNode->getChildRef(pos), value, ref)
        end
    end
```

---

## Iteradores de BinaryTree — Órdenes de recorrido

| Clase | Orden | Resultado en BST ascendente |
|---|---|---|
| `BinaryTreeForwardInorderIterator` | LNR (izq → nodo → der) | ascendente |
| `BinaryTreeBackwardInorderIterator` | RNL (der → nodo → izq) | descendente |
| `BinaryTreeForwardPreorderIterator` | NLR (nodo → izq → der) | raíz primero |
| `BinaryTreeBackwardPreorderIterator` | NRL (nodo → der → izq) | raíz primero, der antes izq |
| `BinaryTreeForwardPostorderIterator` | LRN (izq → der → nodo) | raíz al final |
| `BinaryTreeBackwardPostorderIterator` | RLN (der → izq → nodo) | raíz al final, der antes izq |

> Todos usan `deque<Node*>` pre-construida en el constructor (`construir()` recursivo). `operator++` solo avanza la deque. Coste: O(n) tiempo y espacio. Compatible con `general_iterator` CRTP sin modificar la clase base.

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

> `insertar_interno`, `copiar_interno`, `escribir_interno` no adquieren lock propio — siempre llamados bajo lock del método público.

---

## Patrones de diseño utilizados

| Patrón | Dónde |
|--------|-------|
| **CRTP** (Curiously Recurring Template Pattern) | `general_iterator<Container, IteratorBase>` |
| **Nodo anidado** (nested Node) | `struct Node` dentro de `LinkedList`, `DoubleLinkedList`, `BinaryTree` |
| **Traits** | solo `Vector` y `Heap` — LL/DLL/BT usan `template<T, Comp>` directo |
| **RAII** | `scoped_lock<mutex>` en todas las operaciones mutantes |
| **Move semantics** | constructores y operadores de movimiento en `LinkedList`, `BinaryTree` |
| **Variadic templates** | `ForEach`, `FirstThat` con `Args&&...` |
| **Perfect forwarding** | `std::forward<Args>(args)...` en ForEach/FirstThat |
