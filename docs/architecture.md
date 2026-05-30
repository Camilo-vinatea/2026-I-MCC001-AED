# Arquitectura — EST-DATOS-MCC-001

Documentación de todas las clases, jerarquías y relaciones del repositorio.

---

## Jerarquía de contenedores

```mermaid
classDiagram
    class LinkedList~Traits~ {
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
        #internal_insert(pNode&, value, ref, pPrev)*
    }

    class DoubleLinkedList~Traits~ {
        +push_front(value, ref)
        +push_back(value, ref)
        +begin() forward_iterator
        +end() forward_iterator
        +rbegin() backward_iterator
        +rend() backward_iterator
        +FirstThat(func, args) forward_iterator
        #internal_insert(pNode&, value, ref, pPrev)
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
        +post_begin() forward_postorder_iterator
        +ForEach(func, args)
        +ReverseForEach(func, args)
        +ForEachPreorder(func, args)
        +ForEachReversePreorder(func, args)
        +ForEachPostorder(func, args)
        +ForEachReversePostorder(func, args)
        +FirstThat(func, args) forward_inorder_iterator
        +ReverseFirstThat(func, args) backward_inorder_iterator
        #internal_insert(pNode&, value, ref)
        #internal_copy(dst&, src)
        #internal_write(os, node)
    }

    LinkedList <|-- DoubleLinkedList : hereda
    LinkedList <|-- CircularLinkedList : hereda
    DoubleLinkedList <|-- CircularDoubleLinkedList : hereda
```

---

## Jerarquía de Nodos

```mermaid
classDiagram
    class LLNode~T~ {
        #value_type m_data
        #Ref m_ref
        #Node* m_pNext
        +getData() value_type
        +getDataRef() value_type&
        +setData(data)
        +getRef() Ref
        +getRefRef() Ref&
        +setRef(ref)
        +getNext() Node*
        +getNextRef() Node*&
        +setNext(pNext)
    }

    class DLLNode~T~ {
        -Node* m_pPrev
        +getNext() Node*
        +getNextRef() Node*&
        +setNext(pNext)
        +getPrev() Node*
        +getPrevRef() Node*&
        +setPrev(pPrev)
    }

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

    class BinaryTreeNode~T~ {
        #value_type m_data
        #Ref m_ref
        #NodePtr m_pChild[2]
        #NodePtr m_pParent
        +getData() value_type
        +getDataRef() value_type&
        +getRef() Ref
        +getChild(pos) NodePtr
        +getChildRef(pos) NodePtr&
        +getParent() NodePtr
        +to_string() string
    }

    LLNode <|-- DLLNode : hereda
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

    class DoubleLinkedListForwardIterator~Container~ {
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
        -deque~Node*~ m_queue
        +operator++() MySelf&
    }
    class BinaryTreeBackwardInorderIterator~Container~ {
        -deque~Node*~ m_queue
        +operator++() MySelf&
    }
    class BinaryTreeForwardPreorderIterator~Container~ {
        -deque~Node*~ m_queue
        +operator++() MySelf&
    }
    class BinaryTreeBackwardPreorderIterator~Container~ {
        -deque~Node*~ m_queue
        +operator++() MySelf&
    }
    class BinaryTreeForwardPostorderIterator~Container~ {
        -deque~Node*~ m_queue
        +operator++() MySelf&
    }
    class BinaryTreeBackwardPostorderIterator~Container~ {
        -deque~Node*~ m_queue
        +operator++() MySelf&
    }

    general_iterator <|-- LinkedListForwardIterator : hereda
    general_iterator <|-- DoubleLinkedListForwardIterator : hereda
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

---

## Jerarquía de Traits

```mermaid
classDiagram
    class BaseContainerTrait~_T_Node~ {
        +value_type
        +Node
    }

    class BaseLinkedListTrait~T~ {
    }

    class AscendingLinkedListTrait~T~ {
        +Comp = less~T~
    }

    class DescendingLinkedListTrait~T~ {
        +Comp = greater~T~
    }

    class BaseDoubleLinkedListTrait~T~ {
    }

    class AscendingDoubleLinkedListTrait~T~ {
        +Comp = less~T~
    }

    class DescendingDoubleLinkedListTrait~T~ {
        +Comp = greater~T~
    }

    class VectorTraits~T~ {
    }

    class BaseBinaryTreeListTrait~T~ {
    }
    class AscendingBinaryTreeListTrait~T~ {
        +Comp = less~T~
    }
    class DescendingBinaryTreeListTrait~T~ {
        +Comp = greater~T~
    }

    BaseContainerTrait <|-- BaseLinkedListTrait : hereda
    BaseContainerTrait <|-- BaseDoubleLinkedListTrait : hereda
    BaseContainerTrait <|-- VectorTraits : hereda
    BaseContainerTrait <|-- BaseBinaryTreeListTrait : hereda
    BaseLinkedListTrait <|-- AscendingLinkedListTrait : hereda
    BaseLinkedListTrait <|-- DescendingLinkedListTrait : hereda
    BaseDoubleLinkedListTrait <|-- AscendingDoubleLinkedListTrait : hereda
    BaseDoubleLinkedListTrait <|-- DescendingDoubleLinkedListTrait : hereda
    BaseBinaryTreeListTrait <|-- AscendingBinaryTreeListTrait : hereda
    BaseBinaryTreeListTrait <|-- DescendingBinaryTreeListTrait : hereda
    AscendingTrait <|-- AscendingBinaryTreeListTrait : hereda
    DescendingTrait <|-- DescendingBinaryTreeListTrait : hereda
```

---

## Asociación: Contenedor ↔ Traits ↔ Nodo ↔ Iterador

```mermaid
classDiagram
    direction LR

    class LinkedList~Traits~ {
        Node = Traits::Node
        Comp = Traits::Comp
    }
    class AscendingLinkedListTrait~T~ {
        Node = LLNode~T~
        Comp = less~T~
    }
    class LLNode~T~
    class LinkedListForwardIterator~Container~

    LinkedList --> AscendingLinkedListTrait : usa traits
    AscendingLinkedListTrait --> LLNode : define Node
    LinkedList --> LinkedListForwardIterator : provee begin/end

    class DoubleLinkedList~Traits~ {
        Node = DLLNode~T~
    }
    class AscendingDoubleLinkedListTrait~T~ {
        Node = DLLNode~T~
        Comp = less~T~
    }
    class DLLNode~T~
    class DoubleLinkedListForwardIterator~Container~
    class DoubleLinkedListBackwardIterator~Container~

    DoubleLinkedList --> AscendingDoubleLinkedListTrait : usa traits
    AscendingDoubleLinkedListTrait --> DLLNode : define Node
    DoubleLinkedList --> DoubleLinkedListForwardIterator : begin/end
    DoubleLinkedList --> DoubleLinkedListBackwardIterator : rbegin/rend

    class BinaryTree~Traits~ {
        Node = BinaryTreeNode~T~
        Comp = less~T~ o greater~T~
    }
    class AscendingBinaryTreeListTrait2~T~ {
        Node = BinaryTreeNode~T~
        Comp = less~T~
    }
    class BinaryTreeNode~T~
    class BinaryTreeForwardInorderIterator~Container~
    class BinaryTreeBackwardInorderIterator~Container~

    BinaryTree --> AscendingBinaryTreeListTrait2 : usa traits
    AscendingBinaryTreeListTrait2 --> BinaryTreeNode : define Node
    BinaryTree --> BinaryTreeForwardInorderIterator : begin/end
    BinaryTree --> BinaryTreeBackwardInorderIterator : rbegin/rend
```

---

## Flujo de inserción ordenada (insert)

```mermaid
sequenceDiagram
    participant Cliente
    participant LinkedList
    participant internal_insert

    Cliente->>LinkedList: insert(value, ref)
    LinkedList->>LinkedList: scoped_lock(m_mtx)
    LinkedList->>internal_insert: internal_insert(m_pRoot, value, ref, nullptr)

    loop Recorrido recursivo hasta posición correcta
        internal_insert->>internal_insert: ¿comp(value, pNode->data)?
        alt Sí: insertar aquí
            internal_insert->>internal_insert: pNode = new Node(value, ref, pNode)
            internal_insert->>internal_insert: actualizar m_pTail si es último
        else No: avanzar
            internal_insert->>internal_insert: internal_insert(pNode->next, value, ref, pNode)
        end
    end
```

---

## Flujo de inserción en lista circular (CLL / CDLL)

```mermaid
sequenceDiagram
    participant Cliente
    participant CircularList
    participant BaseInsert as LinkedList/DLL::internal_insert

    Cliente->>CircularList: insert(value, ref)
    CircularList->>CircularList: scoped_lock(m_mtx)
    CircularList->>CircularList: romper link circular (tail->next = nullptr)
    CircularList->>BaseInsert: internal_insert(m_pRoot, value, ref, nullptr)
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
    basetrait["containers/basetrait.h<br/>BaseContainerTrait"]
    geniter["containers/general_iterator.h<br/>general_iterator CRTP"]
    vector["containers/vector.h<br/>Vector + VectorNode + Traits"]
    ll["containers/linkedlist.h<br/>LinkedList + LLNode + Traits"]
    dll["containers/doublelinkedlist.h<br/>DoubleLinkedList + DLLNode + Traits"]
    cll["containers/circularlinkedlist.h<br/>CircularLinkedList"]
    cdll["containers/circulardoublelinkedlist.h<br/>CircularDoubleLinkedList"]

    DemoLL["DoubleLinkedListDemo.cpp"]
    DemoCLL["CircularLinkedListDemo.cpp"]
    DemoCDLL["CircularDoubleLinkedListDemo.cpp"]

    bt["containers/binarytree.h<br/>BinaryTree + BinaryTreeNode + Traits"]
    DemoBT["DemoBinaryTree.cpp"]

    main --> lists
    main --> macros
    main --> bt
    DemoLL --> dll
    DemoCLL --> cll
    DemoCDLL --> cdll
    DemoBT --> bt

    dll --> ll
    cll --> ll
    cdll --> dll

    ll --> geniter
    ll --> basetrait
    ll --> types
    ll --> foreach
    vector --> geniter
    vector --> basetrait
    vector --> types
    bt --> geniter
    bt --> basetrait
    bt --> types
    bt --> foreach
```

---

## Operadores de stream

| Clase | `operator<<` | `operator>>` |
|-------|-------------|--------------|
| `VectorNode` | imprime `(data, ref)` | — |
| `Vector` | imprime `[n0, n1, ...]` | — |
| `LLNode` | imprime `(data, ref)` | — |
| `LinkedList` | llama `toString()` | `push_back` desde stream |
| `DoubleLinkedList` | delega a `LinkedList::operator<<` | delega a `LinkedList::operator>>` |
| `BinaryTreeNode` | imprime `Node(data: X, ref: Y)` | lee `data ref` por línea |
| `BinaryTree` | escribe `n` + nodos en preorden (`data ref\n`) | lee conteo + `data ref` → `insert()` |

---

## Concurrencia

Todos los contenedores usan `std::mutex m_mtx` (heredado desde `LinkedList` / en `Vector` / `mutable` en `BinaryTree`).

| Operación | Mecanismo |
|-----------|-----------|
| `push_front / push_back` | `scoped_lock<mutex>` |
| `pop_front / pop_back` | `scoped_lock<mutex>` |
| `insert` | `scoped_lock<mutex>` en el punto de entrada |
| `toString` (LinkedList) | sin lock — uso interno |
| `ForEach` (LinkedList) | `unique_lock<mutex>` |
| `Vector::push_back` | `scoped_lock<mutex>` |
| `Vector::resize` | `scoped_lock<mutex>` |
| `Vector::ToString` | `scoped_lock<mutex>` |
| `BinaryTree::insert` | `scoped_lock<mutex>` |
| `BinaryTree::ForEach` / `ReverseForEach` / variantes | `scoped_lock<mutex>` |
| `BinaryTree::FirstThat` / `ReverseFirstThat` | `scoped_lock<mutex>` |
| `BinaryTree::toString` | `scoped_lock<mutex>` |
| `BinaryTree::operator<<` | `scoped_lock<mutex>` |
| `BinaryTree` copy/move constructors | `scoped_lock<mutex>` sobre `other.m_mtx` |
| `BinaryTree` destructor | `scoped_lock<mutex>` |

> `internal_insert`, `internal_copy`, `internal_write` no adquieren lock propio — siempre llamados bajo lock del método público.

---

## Flujo de inserción en BST (BinaryTree)

```mermaid
sequenceDiagram
    participant Cliente
    participant BinaryTree
    participant internal_insert

    Cliente->>BinaryTree: insert(value, ref)
    BinaryTree->>BinaryTree: scoped_lock(m_mtx)
    BinaryTree->>internal_insert: internal_insert(m_pRoot, value, ref)

    loop Descenso recursivo BST
        internal_insert->>internal_insert: ¿pNode == nullptr?
        alt Sí: posición encontrada
            internal_insert->>internal_insert: pNode = new Node(value, ref)
            internal_insert->>internal_insert: ++m_size
        else No: elegir hijo
            internal_insert->>internal_insert: pos = !comp(value, pNode->data)
            internal_insert->>internal_insert: internal_insert(pNode->getChildRef(pos), value, ref)
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

> Todos usan `deque<Node*>` pre-construida en el constructor (`build()` recursivo). `operator++` solo avanza la deque. Coste de construcción: O(n) tiempo y espacio. Compatible con el patrón `general_iterator` CRTP sin modificar la clase base.

---

## Patrones de diseño utilizados

| Patrón | Dónde |
|--------|-------|
| **CRTP** (Curiously Recurring Template Pattern) | `general_iterator<Container, IteratorBase>` |
| **Traits** | `AscendingLinkedListTrait`, `VectorTraits`, etc. |
| **NVI** (Non-Virtual Interface) | `insert` → `internal_insert` virtual |
| **RAII** | `scoped_lock<mutex>` en todas las operaciones mutantes |
| **Move semantics** | constructores y operadores de movimiento en `LinkedList`, `BinaryTree` |
| **Variadic templates** | `ForEach`, `FirstThat` con `Args&&...` |
| **Perfect forwarding** | `std::forward<Args>(args)...` en ForEach/FirstThat |
