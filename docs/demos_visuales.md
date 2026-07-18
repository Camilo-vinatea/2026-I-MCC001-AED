# Estructuras y demos — Guía visual (Mermaid)

Documento de apoyo para presentación. Cubre cuatro demos del repositorio:

1. [Digital Trie](#1-digital-trie-árbol-de-prefijos)
2. [Tabla Hash y distribución de Poisson](#2-tabla-hash--distribución-de-poisson)
3. [Dijkstra — camino más corto (STL)](#3-dijkstra--camino-más-corto)
4. [LRU Cache (STL)](#4-lru-cache)

Todas usan los alias de tipos del proyecto (`types.h`): `TI=int`, `TD=double`, `TS=string`, `Ref=long`.

---

## 1. Digital Trie (árbol de prefijos)

**Qué resuelve:** almacenar un conjunto de cadenas para buscar por **clave exacta** y por **prefijo** en tiempo proporcional al largo de la clave, no al número de claves. Recorrido en **orden lexicográfico** gratis.

**Contenedor STL clave:** `std::map<char, Node*>` en cada nodo → hijos ordenados por carácter (por eso el recorrido sale ordenado).

### Estructura de clases

```mermaid
classDiagram
    class DigitalTrie~T~ {
        -Node* m_pRoot
        -size_t m_size
        -mutex m_mtx
        +insert(key, value, ref)
        +find(key) Node*
        +contains(key) bool
        +startsWith(prefix) bool
        +erase(key) bool
        +ForEach(func, args)
        +FirstThat(func, args)
        +begin() forward_iterator
    }
    class Node {
        +char m_char
        +T m_data
        +Ref m_ref
        +bool m_isEnd
        +Node* m_pParent
        +map~char,Node*~ m_children
        +getKey() string
        +nextPreorder() Node*
    }
    class DigitalTrieForwardIterator~Container~ {
        +operator++()
    }
    DigitalTrie "1" *-- "muchos" Node : contiene
    Node "1" o-- "0..*" Node : m_children
    DigitalTrie ..> DigitalTrieForwardIterator : begin/end
```

### Ejemplo del demo

Insertando `cat, car, card, dog, do, dot`. Los nodos marcados con `*` son **terminales** (fin de una clave):

```mermaid
graph TD
    root(( raíz )) --> c((c))
    c --> a((a))
    a --> t((t*<br/>cat))
    a --> r((r*<br/>car))
    r --> d((d*<br/>card))
    root --> d0((d))
    d0 --> o((o*<br/>do))
    o --> g((g*<br/>dog))
    o --> t2((t*<br/>dot))
```

Observaciones para exponer:
- `car` y `card` **comparten** el prefijo `car` → no se duplica.
- `startsWith("ca")` es `true` aunque `"ca"` no sea terminal.
- El recorrido preorden entrega: `car, card, cat, do, dog, dot` (lexicográfico).

### Operaciones y costos (n = largo de la clave)

| Operación | Costo | Nota |
|---|---|---|
| `insert` / `find` / `contains` | O(n) | independiente del nº de claves |
| `startsWith(prefix)` | O(largo del prefijo) | |
| `erase` | O(n) | además **poda** nodos hoja no terminales |
| recorrido completo | O(nodos) | orden lexicográfico |

---

## 2. Tabla Hash + distribución de Poisson

**Qué demuestra:** con `m` cubetas, `n` claves y factor de carga `λ = n/m`, la **longitud de cada cadena** (colisiones por cubeta) sigue una **distribución de Poisson**:

$$P(\text{cubeta con } k \text{ claves}) = \frac{e^{-\lambda}\,\lambda^{k}}{k!}$$

### Flujo de inserción

```mermaid
flowchart LR
    K["clave i (secuencial 0..n-1)"] --> H["splitmix64<br/>mezcla de bits"]
    H --> M["índice = hash % m"]
    M --> B["push_back a la cubeta<br/>(chaining)"]
```

El mezclador `splitmix64` convierte claves **ordenadas** en índices **dispersos**; sin él no habría distribución Poisson.

### Observado vs. teórico (m = n = 100000, λ = 1)

```mermaid
xychart-beta
    title "Cubetas con k claves: observado (barra) vs Poisson (línea)"
    x-axis "k = claves por cubeta" [0, 1, 2, 3, 4, 5, 6]
    y-axis "número de cubetas" 0 --> 40000
    bar [36770, 36798, 18365, 6202, 1527, 278, 53]
    line [36788, 36788, 18394, 6131, 1533, 307, 51]
```

La barra (medido) y la línea (Poisson teórico) coinciden → confirma el modelo.

### Puntos para exponer

- `λ = 1` ⇒ ~37% de cubetas vacías (`e^-1 ≈ 0.368`) y ~37% con exactamente 1 clave.
- Colisión = cubeta con `k ≥ 2`. Poisson predice cuántas.
- Un **buen hash** hace que la entrada, aunque ordenada, se comporte como aleatoria uniforme → `Binomial(n, 1/m) ≈ Poisson(λ)`.

---

## 3. Dijkstra — camino más corto

**Qué resuelve:** ruta de **costo mínimo** desde un nodo origen a todos los demás en un grafo ponderado.

**Contenedores STL:**
| Contenedor | Rol |
|---|---|
| `vector<vector<pair<TI,TI>>>` | lista de adyacencia (destino, peso) |
| `priority_queue` (min-heap con `greater`) | extraer siempre el nodo más cercano |
| `vector<TI> dist`, `vector<TI> prev` | distancias y predecesores |

### Grafo de entrada (6 nodos)

```mermaid
graph LR
    n0((0)) ---|7| n1((1))
    n0 ---|9| n2((2))
    n0 ---|14| n5((5))
    n1 ---|10| n2
    n1 ---|15| n3((3))
    n2 ---|11| n3
    n2 ---|2| n5
    n3 ---|6| n4((4))
    n4 ---|9| n5
```

### Algoritmo

```mermaid
flowchart TD
    A["dist[src]=0, resto=∞"] --> B["push src al min-heap"]
    B --> C{"¿heap vacío?"}
    C -->|sí| Z["fin"]
    C -->|no| D["u = extraer mínimo"]
    D --> E{"d > dist[u]?"}
    E -->|sí, entrada obsoleta| C
    E -->|no| F["relajar vecinos de u:<br/>si dist[u]+w < dist[v] → actualizar"]
    F --> C
```

### Resultado: árbol de caminos mínimos (origen = 0)

```mermaid
graph TD
    r0((0<br/>d=0)) --> r1((1<br/>d=7))
    r0 --> r2((2<br/>d=9))
    r2 --> r3((3<br/>d=20))
    r2 --> r5((5<br/>d=11))
    r5 --> r4((4<br/>d=20))
```

Punto para exponer: los nodos **3 y 4** empatan en distancia 20 pero por **rutas distintas** (`0→2→3` vs `0→2→5→4`). El árbol lo hace evidente.

**Complejidad:** O((V + E) · log V). Sin heap sería O(V²).

---

## 4. LRU Cache

**Qué resuelve:** caché de tamaño fijo que, al llenarse, desaloja lo **menos recientemente usado** (Least Recently Used). `get` y `put` en **O(1)**.

**El truco — dos estructuras combinadas:**

```mermaid
flowchart LR
    subgraph IDX["unordered_map: clave → iterador"]
        direction TB
        mA["A"]
        mC["C"]
        mB["B"]
    end
    subgraph LST["list doblemente enlazada (MRU ← → LRU)"]
        direction LR
        nA["A:1"] <--> nC["C:3"] <--> nB["B:2"]
    end
    mA -.-> nA
    mC -.-> nC
    mB -.-> nB
```

- La **lista** da el orden de uso (frente = reciente, cola = candidato a botar).
- El **map** encuentra el nodo en O(1) sin recorrer la lista.
- `list::splice` mueve el nodo usado al frente en O(1) **sin invalidar** el iterador guardado.

### Traza del demo (capacidad 3)

```mermaid
flowchart TD
    O1["put(A) → [A]"] --> O2["put(B) → [B, A]"]
    O2 --> O3["put(C) → [C, B, A]  (lleno)"]
    O3 --> O4["get(A) → [A, C, B]  (A salta al frente)"]
    O4 --> O5["put(D) → desaloja B → [D, A, C]"]
    O5 --> O6["get(B) → MISS  (ya no está)"]
    O6 --> O7["get(C) → [C, D, A]"]
    O7 --> O8["put(E) → desaloja A → [E, C, D]"]
```

Punto para exponer: **usar** un elemento lo salva del desalojo. `B` murió porque nadie lo tocó; `A` sobrevivió más porque `get(A)` lo refrescó, hasta que dejó de usarse.

**Complejidad:** `get` y `put` en O(1) amortizado.

---

## Cierre: lección común

Elegir el contenedor correcto para cada operación crítica:

| Demo | Operación crítica | Contenedor elegido |
|---|---|---|
| Digital Trie | recorrido ordenado por prefijo | `map<char, Node*>` |
| Hash + Poisson | dispersión uniforme | hash (chaining con `vector`) |
| Dijkstra | extraer el mínimo | `priority_queue` |
| LRU Cache | buscar + reordenar en O(1) | `list` + `unordered_map` |
