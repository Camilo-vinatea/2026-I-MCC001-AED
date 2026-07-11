# Demo de `std::map` en C++

> Demo general del contenedor asociativo ordenado `std::map`.
> **Archivo:** `demos/stdmapDemo.cpp`
> **Header:** `containers/mapDemo.h`
> **Estándar:** C++17 (usa `std::map`, structured bindings, `try_emplace`)
> **Aliases:** usa `TI`/`TD`/`TS`/`Ref` de `types.h` para consistencia con el repo
> **Compilación:** ver [Build](#build)

---

## 1. ¿Qué es `std::map`?

`std::map` es un **contenedor asociativo ordenado** que almacena pares
**clave-valor únicos**. Internamente se implementa como un **árbol binario
de búsqueda balanceado** (típicamente Red-Black tree, según la STL).

### Propiedades clave

| Propiedad            | Valor                                          |
|----------------------|------------------------------------------------|
| Estructura interna   | Árbol binario balanceado (Red-Black tree)      |
| Orden                | Por clave (ascendente por defecto)             |
| Unicidad de claves   | Sí — `std::multimap` permite duplicados        |
| Complejidad insert   | **O(log n)** amortizado                        |
| Complejidad búsqueda | **O(log n)**                                   |
| Complejidad borrado  | **O(log n)**                                   |
| Iteradores           | Bidireccionales                                |
| Inserción duplicada  | Rechazada (no sobrescribe con `try_emplace`)   |
| Garantía de orden    | Recorrido in-order siempre ordenado por clave  |

### Cabecera

```cpp
#include <map>
```

### Declaración básica

```cpp
std::map<std::string, int> ages;            // clave string, valor int
std::map<int, std::string, MiComparador> m;  // con comparador personalizado
```

---

## 2. Secciones de la demo

La demo se divide en **11 bloques temáticos**, cada uno enfocado en un
aspecto del API de `std::map`.

---

### 2.1 Inserción básica — `demoBasicInsertion`

Cubre las **5 formas principales** de insertar elementos.

```cpp
ages["Ana"] = 30;                              // operator[]
ages.insert({"Maria", 28});                    // insert con initializer_list
ages.insert(std::pair<std::string,int>(...));  // insert con pair explícito
ages.emplace("Sofia", 22);                     // construye in-place
ages.try_emplace("Juan", 33);                  // no sobrescribe si existe
```

#### Diferencias críticas

| Método           | Sobrescribe si existe | Construye in-place | Retorna                |
|------------------|-----------------------|--------------------|------------------------|
| `operator[]`     | ✅ Sí                 | ❌ No              | Referencia al valor    |
| `insert(pair)`   | ❌ No                 | ❌ No              | `pair<it, bool>`       |
| `emplace(args)`  | ❌ No                 | ✅ Sí              | `pair<it, bool>`       |
| `try_emplace`    | ❌ No                 | ✅ Sí              | `pair<it, bool>`       |

`try_emplace("Ana", 99)` retorna `inserted=0` y conserva el valor
original `30` — **comportamiento clave** para inserciones idempotentes.

---

### 2.2 Búsqueda — `demoLookup`

Cuatro variantes de lookup, todas en **O(log n)**.

```cpp
auto it = m.find("b");           // iterador o m.end()
size_t n = m.count("z");         // 0 o 1 (map es único)
bool has = m.contains("a");      // C++20+, más legible
auto [lo, hi] = m.equal_range("b"); // rango [lo, hi)
```

#### ¿Cuándo usar cada uno?

- **`find`** → necesitas el valor o iterador para modificar.
- **`count`** → solo quieres saber si existe (legado pre-C++20).
- **`contains`** → equivalente a `count(k) > 0` pero más expresivo (C++20).
- **`equal_range`** → principalmente útil en `multimap`; en `map` el
  rango tiene tamaño 0 o 1.

---

### 2.3 Eliminación — `demoErase`

Tres formas de borrar, todas en **O(log n)**.

```cpp
size_t removed = m.erase(2);              // por clave, retorna count borrados
m.erase(it);                              // por iterador
m.erase(begin, end);                      // por rango semi-abierto [begin, end)
```

`erase(2)` retorna `1` si la clave existía, `0` si no. Útil para
detectar presencia + borrado en una sola operación.

---

### 2.4 Iteración — `demoIteration`

Los iteradores de `map` son **bidireccionales** y recorren el árbol
**in-order** (clave ascendente).

```cpp
for (auto it = m.rbegin(); it != m.rend(); ++it) { /* reverse */ }
for (const auto& [k, v] : m)               { /* structured bindings */ }
```

El **reverse iterator** produce claves en orden descendente sin coste
adicional — el árbol está balanceado y los enlaces inversos son
punteros internos.

---

### 2.5 Comparador personalizado — `demoCustomComparator`

`std::map` acepta un tercer parámetro template: el **comparador**.
Por defecto usa `std::less<T>`.

```cpp
struct LenThenLex {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.size() < b.size()
            || (a.size() == b.size() && a < b);
    }
};

std::map<std::string, int, LenThenLex> m;
```

#### Salida esperada (ordenado por longitud, luego alfabético)

```
oso      -> 4      (3 chars)
gato     -> 2      (4 chars)
perro    -> 1      (5 chars)
elefante -> 3      (8 chars)
```

**Regla del comparador:** debe imponer un **orden total estricto**
(strict weak ordering). Si dos elementos son equivalentes según el
comparador, son indistinguibles para el `map`.

---

### 2.6 `lower_bound` / `upper_bound` — `demoBounds`

Navegación eficiente por el árbol en **O(log n)** por llamada.

```cpp
auto lo = m.lower_bound(20);  // primer elemento >= 20
auto hi = m.upper_bound(30);  // primer elemento >  30
// rango [lo, hi) contiene exactamente {20, 30}
```

#### Tabla rápida

| Función          | Semántica                              |
|------------------|----------------------------------------|
| `lower_bound(k)` | Primer elemento con clave `>= k`        |
| `upper_bound(k)` | Primer elemento con clave `>  k`       |
| `equal_range(k)` | Par `[lower_bound, upper_bound)`        |
| `find(k)`        | Iterador al elemento o `end()`         |

Útil para implementar búsquedas por rango sin escanear linealmente.

---

### 2.7 `multimap` vs `map` — `demoMultiVsUnique`

`std::multimap` permite **claves duplicadas**; la API de búsqueda usa
`equal_range` porque un `find` retorna solo el primero.

```cpp
std::multimap<std::string, std::string> mm;
mm.emplace("MX", "CDMX");
mm.emplace("MX", "Guadalajara");  // duplicado permitido
mm.emplace("MX", "Monterrey");

auto range = mm.equal_range("MX");
for (auto it = range.first; it != range.second; ++it) { ... }
```

#### Diferencias de API

| Operación        | `map`            | `multimap`                       |
|------------------|------------------|----------------------------------|
| `count(k)`       | 0 o 1            | n (cualquier valor)               |
| `operator[]`     | ✅ disponible    | ❌ no existe                     |
| `insert`         | Rechaza dup      | Acepta duplicados                |
| `equal_range(k)` | 0 o 1 elemento   | n elementos                      |

---

### 2.8 Inserción masiva — `demoBulkInsertAndComplexity`

Demuestra empíricamente el costo **O(n log n)** de insertar 100,000
elementos. `std::map` **no tiene `reserve`** (a diferencia de `vector`)
porque el árbol crece nodo a nodo con rebalanceos automáticos.

La clave es `TI` y el valor `Ref` (alias `long` de `types.h`).

```cpp
std::map<TI, Ref> m;
for (TI i = 0; i < N; ++i) m.emplace(i, static_cast<Ref>(i));
cout << "m[50000]=" << m[50000] << "\n";   // 50000
```

---

### 2.9 Algoritmos STL sobre `map` — `demoStlAlgorithms`

Los algoritmos de `<algorithm>` funcionan directamente sobre los
iteradores de `map`.

```cpp
std::for_each(m.begin(), m.end(), [](const auto& p) { ... });
std::find_if(m.begin(), m.end(), [](const auto& p) { return p.second >= 90; });
std::any_of / std::all_of(..., [](const auto& p) { ... });
```

⚠️ **No usar** algoritmos que invaliden la estructura (orden, balance):
`std::sort`, `std::reverse`, `std::remove` — el árbol ya está ordenado
y rebalancearlo manualmente rompería las invariantes.

---

### 2.10 Construcción bulk — `demoBulkConstruction`

Construir desde `initializer_list` o rango de pares.

```cpp
std::map<std::string, int> m(v.begin(), v.end());  // desde vector
std::map<std::string, int> m{{"a",1}, {"b",2}};    // initializer_list
```

Cuando hay claves duplicadas en la fuente, `std::map` **conserva el
primer valor encontrado** (no sobrescribe con los siguientes).

---

### 2.11 `map<TD, TD>` con doubles — `demoMapDouble`

Muestra el uso del alias `TD` (`double`) como clave **y** valor. El árbol
ordena por clave flotante ascendente igual que con enteros.

```cpp
std::map<TD, TD> m{{1.5, 2.5}, {3.0, 4.0}, {0.5, 1.0}};
m.emplace(2.0, 3.5);
for (const auto& [k, v] : m) cout << k << " -> " << v << "\n";
```

Salida (ordenada por clave):

```
0.5 -> 1
1.5 -> 2.5
2 -> 3.5
3 -> 4
```

> ⚠️ Usar `double` como clave es válido pero frágil ante errores de
> redondeo: `0.1 + 0.2 != 0.3`. Evita claves derivadas de aritmética
> flotante; prefiere valores exactos o enteros escalados.

---

## 3. Resumen de complejidad

| Operación            | Promedio  | Peor caso | Notas                         |
|----------------------|-----------|-----------|-------------------------------|
| `insert` / `emplace` | O(log n)  | O(log n)  | Garantizado por RB-tree       |
| `find` / `count`     | O(log n)  | O(log n)  |                               |
| `erase`              | O(log n)  | O(log n)  | Por clave, iterador o rango   |
| `lower_bound`        | O(log n)  | O(log n)  |                               |
| `begin()` / `end()`  | O(1)      | O(1)      | Punteros directos al árbol    |
| Tamaño de iteración  | O(n)      | O(n)      | Recorre todos los nodos       |

> Para **O(1) promedio** en lookup usa `std::unordered_map` (hash table),
> a costa de perder el orden por clave.

---

## 4. Cuándo usar `std::map`

### ✅ Usar cuando:

- Necesitas **orden por clave** estable (recorridos in-order).
- Haces **búsquedas por rango** (`lower_bound` / `upper_bound`).
- Las claves son **pocos millones o menos** (el árbol cabe en RAM).
- Necesitas **garantía de O(log n)** en el peor caso (no promedio).
- Insertar/consultar de forma mixta es más común que iterar todo.

### ❌ Evitar cuando:

- Solo necesitas lookup → `std::unordered_map` (hash, O(1) promedio).
- Necesitas duplicados → `std::multimap` o `std::multiset`.
- Las claves son **muy grandes** y la copia es cara → considera
  `std::map<std::string_view, T>` o punteros inteligentes.
- Haces muchas inserciones y luego solo lees → quizás `std::vector`
  ordenado + búsqueda binaria gana en caché.

---

## 5. Errores comunes

### 5.1 Usar `operator[]` para solo consultar

```cpp
// ❌ Inserta "Juan" con valor 0 si no existe (efecto colateral).
if (m["Juan"] > 18) { ... }

// ✅ Solo consulta, sin inserción.
if (auto it = m.find("Juan"); it != m.end() && it->second > 18) { ... }
```

### 5.2 Modificar la clave después de insertar

```cpp
// ❌ Rompe el orden del árbol.
it->first = "OtraCosa";

// ✅ Borra y reinserta.
std::string old_key = it->first;
int value = it->second;
m.erase(it);
m.emplace("OtraCosa", value);
```

### 5.3 Iteradores invalidados

- **`insert` / `emplace`** → invalidan **solo** los iteradores al
  elemento borrado (en C++17+ las referencias a otros elementos se
  mantienen).
- **`erase`** → invalida iteradores y referencias al elemento borrado.

### 5.4 Comparador no estricto

```cpp
// ❌ Define orden NO estricto (dos elementos "iguales" pero distintos).
struct Bad {
    bool operator()(int a, int b) const { return a <= b; }
};

// ✅ Estricto: usa `<`, no `<=`.
struct Good {
    bool operator()(int a, int b) const { return a < b; }
};
```

---

## 6. Build

```powershell
# Desde la raíz del repositorio
cd "C:\Users\CAMILO\MAESTRIA UNI MCC\EST-DATOS-MCC-001"

# Build completo (excluyendo DemoVector.cpp que tiene errores pre-existentes)
g++ -std=c++17 -O2 -Wall -I . main.cpp `
    demos\ListsDemo.cpp `
    demos\DemoBinaryTree.cpp `
    demos\BTreeDemo.cpp `
    demos\DemoAVL.cpp `
    demos\stdmapDemo.cpp `
    -o main.exe

# Ejecutar
.\main.exe
```

### Salida esperada (resumen)

```
=== Insercion basica ===
Ana -> 30
Juan -> 33
Luis -> 25
Maria -> 28
Pedro -> 40
Sofia -> 22

=== Busqueda ===
find('b') = 2
count('z') = 0

=== Comparador personalizado (longitud, luego lex) ===
oso -> 4
gato -> 2
perro -> 1
elefante -> 3

... (11 secciones en total)
```

---

## 7. Referencias

- [cppreference: std::map](https://en.cppreference.com/w/cpp/container/map)
- [cppreference: std::multimap](https://en.cppreference.com/w/cpp/container/multimap)
- [cppreference: std::map::try_emplace](https://en.cppreference.com/w/cpp/container/map/try_emplace)
- *Introduction to Algorithms* (CLRS) — Capítulo 13: Red-Black Trees.
- *Effective STL* (Scott Meyers) — Item 22: Avoid in-place key modification
  in `set` and `map`.