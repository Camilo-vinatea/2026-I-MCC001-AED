//! @file LRUCacheDemo.cpp
//! @brief Demo LRU Cache O(1) con `std::list` + `std::unordered_map`.
//! @details Cache de tamaño fijo con política *Least Recently Used*:
//!            - `std::list<std::pair<K,V>>` mantiene el orden de uso; el frente
//!              es el mas reciente, la cola el candidato a desalojo.
//!            - `std::unordered_map<K, list::iterator>` da acceso O(1) al nodo
//!              de la lista sin recorrerla.
//!            - `list::splice` mueve un nodo al frente en O(1) sin copiar ni
//!              invalidar iteradores (clave del truco).
//!          `get` y `put` son O(1) amortizado.
//! @author Equipo MCC

#include "../containers/lruCacheDemo.h"
#include "../types.h"
#include <list>
#include <unordered_map>
#include <utility>
#include <optional>
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;

namespace {

//! @brief Cache LRU genérica de capacidad fija.
template <typename K, typename V>
class LRUCache {
    using Item = pair<K, V>;
    using ListIt = typename list<Item>::iterator;

    size_t cap_;
    list<Item> order_;                       //!< frente = mas reciente
    unordered_map<K, ListIt> index_;         //!< clave -> nodo en order_

public:
    explicit LRUCache(size_t capacity) : cap_(capacity) {}

    //! @brief Devuelve el valor y lo marca como recién usado. nullopt si no está.
    optional<V> get(const K& key) {
        auto it = index_.find(key);
        if (it == index_.end()) return nullopt;
        order_.splice(order_.begin(), order_, it->second); // O(1) al frente
        return it->second->second;
    }

    //! @brief Inserta/actualiza. Si excede capacidad, desaloja el LRU (cola).
    //! @return clave desalojada (si hubo), para que la demo la muestre.
    optional<K> put(const K& key, const V& value) {
        auto it = index_.find(key);
        if (it != index_.end()) {
            it->second->second = value;
            order_.splice(order_.begin(), order_, it->second);
            return nullopt;
        }
        optional<K> evicted;
        if (index_.size() == cap_) {
            evicted = order_.back().first;   // LRU = cola
            index_.erase(order_.back().first);
            order_.pop_back();
        }
        order_.emplace_front(key, value);
        index_[key] = order_.begin();
        return evicted;
    }

    //! @brief Dibujo ASCII del estado: frente (MRU) a la izquierda.
    TS snapshot() const {
        ostringstream os;
        for (const auto& it : order_) os << "[" << it.first << ":" << it.second << "] ";
        TS s = os.str();
        return (s.empty() ? "(vacio) " : s) + "  MRU->LRU";
    }
};

} // namespace

//! @brief Demo LRU Cache con capacidad 3.
void LRUCacheDemo() {
    cout << "=== LRU Cache (capacidad 3) ===\n";
    LRUCache<TS, TI> cache(3);

    auto line = [&](const TS& op) {
        cout << op << "\n      " << cache.snapshot() << "\n";
    };
    auto put = [&](const TS& k, TI v) {
        auto ev = cache.put(k, v);
        line("put(" + k + "," + to_string(v) + ")"
             + (ev ? "  [desaloja '" + *ev + "']" : ""));
    };
    auto get = [&](const TS& k) {
        auto r = cache.get(k);
        line("get(" + k + ") -> " + (r ? to_string(*r) : "MISS"));
    };

    put("A", 1);
    put("B", 2);
    put("C", 3);              // cache: C B A
    get("A");                 // usa A -> cache: A C B
    put("D", 4);              // lleno: desaloja LRU = B -> cache: D A C
    get("B");                 // MISS (fue desalojado)
    get("C");                 // HIT
    put("E", 5);              // desaloja LRU = A -> cache: E C D

    //! @test Auto-verificación de la política LRU.
    assert(!cache.get("B") && "B debio ser desalojado");
    assert(cache.get("D") && "D debe seguir en cache");
    cout << "\nOK: B desalojado, D presente. Politica LRU correcta.\n";
}
