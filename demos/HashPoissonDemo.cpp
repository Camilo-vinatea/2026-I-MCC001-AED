//! @file HashPoissonDemo.cpp
//! @brief Demo: tabla hash con encadenamiento (chaining) que evidencia la
//!        distribución de Poisson en la longitud de las cadenas (colisiones).
//! @details Con `m` cubetas y `n` claves y factor de carga `lambda = n/m`, el
//!          número de claves por cubeta es `Binomial(n, 1/m)`, que para `m`
//!          grande converge a `Poisson(lambda)`:
//!              P(cubeta con k claves) = e^{-lambda} * lambda^k / k!
//!          La demo inserta claves *secuenciales* (0..n-1) mezcladas con un
//!          buen hash (splitmix64) para mostrar que, aun con entrada ordenada,
//!          un hash que dispersa bien produce colisiones ~Poisson.
//! @author Equipo MCC

#include "../containers/hashPoissonDemo.h"
#include <cstdint>
#include <vector>
#include <cmath>
#include <iostream>
#include <iomanip>

using namespace std;

//! @brief Mezclador splitmix64: dispersa bits de una clave entera.
static uint64_t splitmix64(uint64_t x) {
    x += 0x9E3779B97F4A7C15ULL;
    x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
    return x ^ (x >> 31);
}

//! @brief Demo tabla hash con encadenamiento vs. Poisson teórico.
void HashPoissonDemo() {
    const size_t m = 100000;          //!< número de cubetas
    const size_t n = 100000;          //!< número de claves insertadas
    const double lambda = double(n) / double(m); //!< factor de carga

    //! @test Inserción: clave secuencial i -> cubeta splitmix64(i) % m.
    vector<vector<uint32_t>> table(m);
    for (uint32_t i = 0; i < n; ++i)
        table[splitmix64(i) % m].push_back(i);

    //! @test Medición: observed[k] = nº de cubetas con exactamente k claves.
    size_t maxLen = 0;
    for (auto& bucket : table) maxLen = max(maxLen, bucket.size());
    vector<size_t> observed(maxLen + 1, 0);
    for (auto& bucket : table) observed[bucket.size()]++;

    //! @test Predicción: expected[k] = m * e^{-lambda} * lambda^k / k!.
    //! pmf recurrente: p(0)=e^{-lambda}, p(k)=p(k-1)*lambda/k.
    cout << fixed << setprecision(1);
    cout << "Tabla hash: m=" << m << " cubetas, n=" << n
         << " claves, lambda=n/m=" << setprecision(3) << lambda << "\n\n";
    cout << setprecision(1);
    cout << " k |  observado |   Poisson | histograma (observado)\n";
    cout << "---+------------+-----------+" << string(40, '-') << "\n";

    double pmf = exp(-lambda);
    const double scale = 40.0 / observed[0]; // barra relativa al pico
    for (size_t k = 0; k <= maxLen; ++k) {
        double expected = m * pmf;
        size_t bars = size_t(observed[k] * scale + 0.5);
        cout << setw(2) << k << " | "
             << setw(10) << observed[k] << " | "
             << setw(9) << expected << " | "
             << string(bars, '#') << "\n";
        pmf *= lambda / double(k + 1); // avanza a p(k+1)
    }

    //! @test Métricas resumen.
    cout << "\nCadena mas larga: " << maxLen << " claves\n";
    cout << "Cubetas vacias observadas: " << observed[0]
         << " | Poisson: " << m * exp(-lambda) << "\n";
    cout << "Colisiones (cubetas con k>=2): ";
    size_t coll = 0;
    for (size_t k = 2; k <= maxLen; ++k) coll += observed[k];
    cout << coll << "\n";
}
