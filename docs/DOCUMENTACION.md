# Documentación — `EST-DATOS-MCC-001`

Documentación generada para los contenedores y demos del repositorio. El código ya incluye bloques Doxygen (`@file`, `@class`, `@brief`, `@tparam`, `@param`, `@return`, `@warning`, etc.). Para producir la salida HTML:

```bash
doxygen Doxyfile
```

El archivo `Doxyfile` está en la raíz del repositorio y produce la salida en `docs/html/index.html`.

> **Diagrama unificado:** [`jerarquia_clases.md`](jerarquia_clases.md) contiene un único `classDiagram` Mermaid con toda la librería (iteradores CRTP, traits, listas, BST + AVL, B-Tree). Es interactivo en GitHub y en VS Code (extensión Mermaid).
>
> **Diagramas SVG interactivos de Doxygen:** activar Graphviz (`dot` en `PATH`) antes de `doxygen Doxyfile`. Sin dot, Doxygen produce versiones simplificadas con su renderer interno.

---

## Índice de archivos documentados

### Cabeceras comunes (raíz)

| Archivo | Responsabilidad |
|---------|-----------------|
| `types.h` | Aliases portables (`TI`, `TD`, `TS`, `Ref`, `XT`) y tipos específicos del B-Tree (`obj_id_t`, `tree_order_t`, `tree_height_t`, `btree_key_t`, `btree_pos_t`, `use_count_t`). |
| `foreach.h` | Funciones plantilla `ForEach` y `FirstThat` sobre pares de iteradores. |
| `macros.h` / `macros.cpp` | Macros de ejemplo `X`, `SUMA`, `MULT`, `MAX`, `CUADRADO`, `GetVar` y demo. |
| `main.cpp` | Punto de entrada — actualmente invoca `BTreeDemo()`. |

### Contenedores (`containers/`)

| Archivo | Tipo | Notas clave |
|---------|------|-------------|
| `general_iterator.h` | Iterador base (CRTP) | `operator==/!=`, `*`, `->`, asignación. |
| `basetrait.h` | Traits | `BaseContainerTrait`, `AscendingTrait`, `DescendingTrait`. |
| `vector.h` | Contenedor | Memoria contigua dinámica. `push_back`, `ForEach`, `FirstThat`, `Mutex`. |
| `heap.h` | Contenedor | Min-heap/max-heap con `heapify_up`/`heapify_down`. |
| `linkedlist.h` | Contenedor | Lista enlazada simple ordenada con nodo anidado. |
| `doublelinkedlist.h` | Contenedor | Hereda de `LinkedList`; añade `m_pPrev` e iterador backward. |
| `binarytree.h` | Contenedor | BST con nodo anidado, 6 políticas de recorrido, `BinaryTreeRange` para `range-for`. |
| `avl.h` | Contenedor | `AVL<Traits>` hereda de `BinaryTree` y rebalancea con rotaciones LL/LR/RR/RL. |
| `BTreePage.h` | Contenedor (página) | Núcleo del B-Tree: `CBTreePage`, `SplitRoot/SplitChild`, `RedistributeL2R/R2L`, `Merge`, `MergeRoot`, `Call` (motor único de recorrido). |
| `BTree.h` | Contenedor | B-Tree completo: `Insert/Remove/Search`, iteradores `forward/backward`, `ForEach`/`FirstThat` con `scoped_lock`. |

### Demos (`demos/`)

| Archivo | Demo |
|---------|------|
| `BTreeDemo.cpp` | Inserta ~62 claves y muestra 6 formas distintas de recorrerlo. |
| `DemoAVL.cpp` | Inserción 1..7 con verificación de altura y comparador descendente. |
| `DemoBinaryTree.cpp` | BST ascendente: inserciones, 6 recorridos, copia/movimiento, persistencia en `binarytree.txt`. |
| `DemoVector.cpp` | Vector con `int`/`double`/`string` y demo concurrente (5 hilos). |
| `ListsDemo.cpp` | Lista enlazada ascendente y descendente: inserciones, push/pop, copia/movimiento, E/S y `operator[]`. |

---

## Patrones de diseño observados

| Patrón | Dónde |
|--------|-------|
| **Template Method** | `BinaryTree::make_node` / `post_insert` sobreescritos por `AVL`. |
| **Strategy (Policy)** | `BinaryTree*Policy` (6) y `BTreeForward/BackwardInorderPolicy`. |
| **CRTP** | `general_iterator<Container, Derived>` → `BinaryTreeIterator`, `BTreeIterator`, `LinkedListForwardIterator`, `DoubleLinkedListBackwardIterator`, `vector_forward/backward_iterator`. |
| **Range adapter** | `BinaryTreeRange<Container, Policy>` para `for(auto& n : bt.inorder())`. |
| **Pimpl doble (forward friend)** | `BTree::ForwardPolicy` amiga de `CBTreePage` para romper la dependencia circular. |
| **Fold genérico variádico** | `ForEach` / `FirstThat` con `forward<Args>(args)...`. |

---

## Resumen de concurrencia

| Contenedor | Mecanismo | Notas |
|------------|-----------|-------|
| `Vector` | `mutex m_mtx` + `scoped_lock` | `ForEach` no bloquea (puede entrelazarse con `push_back`). |
| `Heap` | Sin sincronización | Hilo único. |
| `LinkedList` | `mutex m_mtx` (`mutable`) + `scoped_lock/unique_lock` | `ForEach` usa `unique_lock` para mantener el mutex todo el recorrido. |
| `DoubleLinkedList` | Idem | `ForEach` usa `unique_lock`; `ReverseForEach` usa `scoped_lock`. |
| `BinaryTree` | `mutex m_mtx` (`mutable`) + `scoped_lock` | `BinaryTreeRange` usa `unique_lock`. |
| `AVL` | Hereda del padre + `scoped_lock` | Idem. |
| `BTree` | `std::shared_mutex m_Mutex` + `scoped_lock` | Las operaciones públicas son exclusivas; `ForEach` está bajo `scoped_lock`. |

---

## Decisiones arquitectónicas relevantes

1. **BTree**: el recorrido se materializa al construir el iterador (cola de pares `(Node*, level)`) en vez de tener un `Call` recursivo. Esto rompe la dependencia circular y permite que `ForEach` y `FirstThat` compartan el mismo motor (`Call`) gracias a `if constexpr (is_void_v<Result>)`.
2. **AVL** hereda de `BinaryTree` para reusar inserciones y políticas: solo cambia `make_node` (devuelve `AVLNode`) y `post_insert` (llama a `rebalance`).
3. **Políticas en B-Tree**: `BTreeForwardInorderPolicy` es amiga de `CBTreePage` y accede directamente a `m_Keys`/`m_SubPages`, evitando pasar por `ForEach` (lo que volvería a invocar la propia política).
4. **Iteradores forward/backward** se modelan con CRTP: `general_iterator` no conoce el incremento concreto; cada derivado define `operator++`.

---

## Build rápido

```bash
g++ -std=c++2b main.cpp -o main.exe      # ejecuta BTreeDemo por defecto
./main.exe
```

`main.cpp` está cableado a `BTreeDemo()`. Las otras demos se activan descomentando las líneas correspondientes dentro de `main`.
