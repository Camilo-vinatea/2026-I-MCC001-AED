//! @file stdmapDemo.cpp
//! @brief Demo general de `std::map` (contenedor asociativo ordenado, llave unica).
//! @details Usa los aliases de `types.h` (TI, TD, TS, Ref) para mantener
//!          consistencia con el resto de demos del repositorio.
//! @author Equipo MCC

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "../types.h"

using namespace std;

namespace {

void section(const string& title) {
    cout << "\n=== " << title << " ===\n";
}

// 1. Insercion basica: operator[], insert(), emplace(), try_emplace().
void demoBasicInsertion() {
    section("Insercion basica");
    map<TS, TI> ages;

    ages["Ana"] = 30;                  // operator[]: inserta default (0) si no existe, luego asigna.
    ages["Luis"] = 25;
    ages.insert({"Maria", 28});        // insert con initializer_list.
    ages.insert(pair<TS, TI>("Pedro", 40));
    ages.emplace("Sofia", 22);         // construye in-place, evita copia.
    ages.try_emplace("Juan", 33);      // no sobreescribe si la llave ya existe.

    auto [it, inserted] = ages.try_emplace("Ana", 99); // NO sobreescribe.
    cout << "try_emplace Ana inserted=" << inserted
         << " value=" << it->second << "\n";

    for (const auto& [name, age] : ages) {
        cout << name << " -> " << age << "\n";
    }
}

// 2. Busqueda: find(), count(), contains() (C++20), equal_range().
void demoLookup() {
    section("Busqueda");
    map<TS, TI> m{{"a", 1}, {"b", 2}, {"c", 3}};

    if (auto it = m.find("b"); it != m.end()) {
        cout << "find('b') = " << it->second << "\n";
    }

    cout << "count('z') = " << m.count("z") << "\n";       // 0 o 1.
#if __cplusplus >= 202002L
    cout << "contains('a') = " << boolalpha << m.contains("a") << "\n";
#endif

    auto [lo, hi] = m.equal_range("b"); // rango [b, c) porque es unico por llave.
    for (auto it = lo; it != hi; ++it) {
        cout << "range: " << it->first << "=" << it->second << "\n";
    }
}

// 3. Eliminacion: erase() por llave, iterador, rango.
void demoErase() {
    section("Eliminacion");
    map<TI, TS> m{{1, "uno"}, {2, "dos"}, {3, "tres"}, {4, "cuatro"}};

    size_t removed = m.erase(2);                       // por llave.
    cout << "erase(2) removed=" << removed << "\n";

    auto it = m.find(3);
    if (it != m.end()) m.erase(it);                   // por iterador.

    m.erase(m.begin(), m.find(4));                    // por rango [begin, 4).

    for (const auto& [k, v] : m) cout << k << ":" << v << "\n";
}

// 4. Iteracion: bidireccional, en orden por llave.
void demoIteration() {
    section("Iteracion");
    map<TI, TS> m{{3, "tres"}, {1, "uno"}, {4, "cuatro"}, {2, "dos"}};

    cout << "Orden natural (reverse iter): ";
    for (auto it = m.rbegin(); it != m.rend(); ++it) {
        cout << it->first << " ";
    }
    cout << "\n";

    // Structured bindings (C++17).
    for (const auto& [k, v] : m) {
        cout << k << "=" << v << " ";
    }
    cout << "\n";
}

// 5. Comparador personalizado: ordenar por longitud de string, luego lex.
struct LenThenLex {
    bool operator()(const TS& a, const TS& b) const {
        return a.size() < b.size() || (a.size() == b.size() && a < b);
    }
};

void demoCustomComparator() {
    section("Comparador personalizado (longitud, luego lex)");
    map<TS, TI, LenThenLex> m;
    m["perro"] = 1;
    m["gato"] = 2;
    m["elefante"] = 3;
    m["oso"] = 4;

    for (const auto& [k, v] : m) cout << k << " -> " << v << "\n";
}

// 6. lower_bound / upper_bound: navegacion eficiente O(log n).
void demoBounds() {
    section("lower_bound / upper_bound");
    map<TI, TS> m{{10, "A"}, {20, "B"}, {30, "C"}, {40, "D"}};

    auto lo = m.lower_bound(20); // primer >= 20.
    auto hi = m.upper_bound(30); // primer <= 30.
    cout << "[lower_bound(20), upper_bound(30)): ";
    for (auto it = lo; it != hi; ++it) cout << it->first << " ";
    cout << "\n";
}

// 7. multimap vs map: llaves duplicadas vs unicas.
void demoMultiVsUnique() {
    section("multimap: llaves duplicadas");
    multimap<TS, TS> mm;
    mm.emplace("MX", "CDMX");
    mm.emplace("MX", "Guadalajara");
    mm.emplace("MX", "Monterrey");
    mm.emplace("AR", "Buenos Aires");

    auto range = mm.equal_range("MX");
    for (auto it = range.first; it != range.second; ++it) {
        cout << it->first << " :: " << it->second << "\n";
    }
}


// 8. Algoritmos STL sobre map: for_each, find_if, any_of/all_of.
void demoStlAlgorithms() {
    section("Algoritmos STL sobre map");
    map<TS, TI> scores{{"ana", 80}, {"luis", 95}, {"sofia", 70}};

    // for_each sobre pares.
    for_each(scores.begin(), scores.end(), [](const auto& p) {
        cout << p.first << "=" << p.second << " ";
    });
    cout << "\n";

    // find_if: primer score >= 90.
    auto it = find_if(scores.begin(), scores.end(),
                      [](const auto& p) { return p.second >= 90; });
    if (it != scores.end()) cout << "top: " << it->first << "=" << it->second << "\n";

    // any_of / all_of.
    bool any_pass = any_of(scores.begin(), scores.end(),
                           [](const auto& p) { return p.second >= 60; });
    bool all_pass = all_of(scores.begin(), scores.end(),
                           [](const auto& p) { return p.second >= 60; });
    cout << boolalpha
         << "any_pass=" << any_pass << " all_pass=" << all_pass << "\n";
}

// 9. Insercion desde otro contenedor / initializer_list.
void demoBulkConstruction() {
    section("Construccion bulk");
    vector<pair<TS, TI>> v{{"x", 1}, {"y", 2}, {"x", 99}};
    map<TS, TI> m(v.begin(), v.end()); // "x" duplicado -> conserva el primero (1).
    for (const auto& [k, v] : m) cout << k << "=" << v << "\n";

    map<TS, TI> m2{{"a", 1}, {"b", 2}}; // initializer_list.
    cout << "m2 size=" << m2.size() << "\n";
}

// 10. Map<double,double> usando TD para mostrar uso del alias de coma flotante.
void demoMapDouble() {
    section("map<TD, TD>: doubles como clave y valor");
    map<TD, TD> m{{1.5, 2.5}, {3.0, 4.0}, {0.5, 1.0}};
    m.emplace(2.0, 3.5);

    for (const auto& [k, v] : m) cout << k << " -> " << v << "\n";
}

} // namespace

//! @brief Punto de entrada unico del demo de std::map.
void stdmapDemo() {
    demoBasicInsertion();
    demoLookup();
    demoErase();
    demoIteration();
    demoCustomComparator();
    demoBounds();
    demoMultiVsUnique();
    demoStlAlgorithms();
    demoBulkConstruction();
    demoMapDouble();

    cout << "\nFin demo std::map.\n";
}