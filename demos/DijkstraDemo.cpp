//! @file DijkstraDemo.cpp
//! @brief Demo Dijkstra: camino mas corto en grafo ponderado con STL.
//! @details Muestra el uso combinado de:
//!            - `std::vector<std::vector<std::pair<TI,TI>>>` : lista de
//!              adyacencia (destino, peso). `TI` = alias de `types.h`.
//!            - `std::priority_queue` como min-heap (con `std::greater`) para
//!              extraer siempre el nodo con distancia tentativa mínima.
//!            - `std::vector` para distancias y predecesores.
//!          Calcula distancias mínimas desde un origen y reconstruye el camino
//!          nodo a nodo. Complejidad O((V+E) log V).
//! @author Equipo MCC

#include "../containers/dijkstraDemo.h"
#include "../types.h"
#include <vector>
#include <queue>
#include <utility>
#include <limits>
#include <iostream>
#include <cassert>

using namespace std;

namespace {

using Edge  = pair<TI, TI>;             //!< (destino, peso) con alias del proyecto
using Graph = vector<vector<Edge>>;     //!< adyacencia por nodo

constexpr TI INF = numeric_limits<TI>::max();

//! @brief Dijkstra clásico. Devuelve (dist, prev) desde `src`.
pair<vector<TI>, vector<TI>> dijkstra(const Graph& g, TI src) {
    vector<TI> dist(g.size(), INF), prev(g.size(), -1);
    dist[src] = 0;

    // min-heap: pares (distancia, nodo); greater => cima = menor distancia.
    priority_queue<Edge, vector<Edge>, greater<Edge>> pq;
    pq.push({0, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;          // entrada obsoleta (lazy deletion)
        for (auto [v, w] : g[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    return {dist, prev};
}

//! @brief Reconstruye camino src->dst usando el vector de predecesores.
TS buildPath(const vector<TI>& prev, TI dst) {
    vector<TI> path;
    for (TI at = dst; at != -1; at = prev[at]) path.push_back(at);
    TS s;
    for (auto it = path.rbegin(); it != path.rend(); ++it)
        s += to_string(*it) + (it + 1 != path.rend() ? " -> " : "");
    return s;
}

//! @brief Diagrama ASCII fijo del grafo de la demo (hecho a mano).
//! ponytail: dibujo cableado a este grafo; si cambian las aristas, actualizar.
void drawGraph() {
    cout <<
        "        15                                                 \n"
        "   (1)------(3)          aristas (nodo-nodo:peso):         \n"
        "  / |        | \\         0-1:7  0-2:9  0-5:14  1-2:10       \n"
        " 7  |10    11|  6        1-3:15 2-3:11 2-5:2   3-4:6        \n"
        "/   |        |   \\       4-5:9                             \n"
        "(0) |        |  (4)                                        \n"
        "\\   |        |  /                                          \n"
        " 9  |        | 9                                           \n"
        "  \\ |        |/                                            \n"
        "   (2)------(5)                                            \n"
        "        2                                                  \n"
        "  (ademas arista directa 0-5 con peso 14)                 \n";
}

//! @brief Imprime el arbol de caminos minimos (children construido desde prev).
void printTree(const vector<vector<TI>>& children, const vector<TI>& dist,
               TI u, const TS& prefix) {
    for (size_t i = 0; i < children[u].size(); ++i) {
        TI v = children[u][i];
        bool last = (i + 1 == children[u].size());
        cout << prefix << (last ? "\\-- " : "|-- ")
             << v << " (dist=" << dist[v] << ")\n";
        printTree(children, dist, v, prefix + (last ? "    " : "|   "));
    }
}

//! @brief Barra proporcional a la distancia.
TS bar(TI d, TI maxD, TI width = 30) {
    TI n = (maxD == 0) ? 0 : TI(double(d) / maxD * width + 0.5);
    return TS(n, '#') + TS(width - n, '.');
}

} // namespace

//! @brief Demo Dijkstra sobre un grafo de 6 nodos.
void DijkstraDemo() {
    //! @test Grafo dirigido ponderado (0..5).
    Graph g(6);
    auto addEdge = [&](TI u, TI v, TI w) {
        g[u].push_back({v, w});
        g[v].push_back({u, w});          // no dirigido
    };
    addEdge(0, 1, 7);
    addEdge(0, 2, 9);
    addEdge(0, 5, 14);
    addEdge(1, 2, 10);
    addEdge(1, 3, 15);
    addEdge(2, 3, 11);
    addEdge(2, 5, 2);
    addEdge(3, 4, 6);
    addEdge(4, 5, 9);

    const TI src = 0;
    cout << "=== Dijkstra (origen = " << src << ") ===\n\n";
    drawGraph();

    auto [dist, prev] = dijkstra(g, src);

    //! @test Tabla: distancia + camino + barra proporcional.
    TI maxD = 0;
    for (TI d : dist) maxD = max(maxD, d);
    cout << "\nnodo | dist | " << string(30, ' ') << " | camino\n";
    cout << "-----+------+" << string(32, '-') << "+--------\n";
    for (size_t v = 0; v < g.size(); ++v) {
        cout << "  " << v << "  |  " << dist[v] << (dist[v] < 10 ? "  " : " ")
             << " | " << bar(dist[v], maxD) << " | "
             << buildPath(prev, TI(v)) << "\n";
    }

    //! @test Arbol de caminos minimos (spanning tree) desde prev.
    vector<vector<TI>> children(g.size());
    for (size_t v = 0; v < g.size(); ++v)
        if (prev[v] != -1) children[prev[v]].push_back(TI(v));
    cout << "\nArbol de caminos minimos:\n" << src << " (raiz)\n";
    printTree(children, dist, src, "");

    //! @test Auto-verificación: distancia conocida 0->4 = 20 (0->2->5->4).
    assert(dist[4] == 20 && "Dijkstra: distancia 0->4 debe ser 20");
    cout << "\nOK: dist(0->4) = " << dist[4] << " (esperado 20)\n";
}
