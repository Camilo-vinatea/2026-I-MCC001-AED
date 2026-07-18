//! @file DigitalTrieDemo.cpp
//! @brief Demo de `DigitalTrie`: insert, contains, startsWith, erase,
//!        `ForEach`, `FirstThat`, iteradores, copia y movimiento.
//! @author Equipo MCC

#include "../containers/digitalTrie.h"

using namespace std;

//! @brief Imprime el nodo (clave, data, ref) seguido de salto de línea.
template <typename Node>
void PrintKey(Node& node, ostream& os) {
    os << "  " << node << "\n";
}

//! @brief Predicado: ¿la clave tiene longitud mayor que `n`?
template <typename Node>
bool KeyLongerThan(Node& node, size_t n) {
    return node.getKey().size() > n;
}

//! @brief Demo completa del `DigitalTrie`.
void DigitalTrieDemo() {
    //! @test Inserción de palabras (clave string -> valor int).
    DigitalTrie<TI> trie;
    trie.insert("cat", 1, 10);
    trie.insert("car", 2, 20);
    trie.insert("card", 3, 30);
    trie.insert("dog", 4, 40);
    trie.insert("do", 5, 50);
    trie.insert("dot", 6, 60);

    cout << "Trie (orden lexicografico): " << trie << endl;
    cout << "Tamano: " << trie.size() << endl;

    //! @test Recorrido con iteradores (range-for implícito vía begin/end).
    cout << "Recorrido con iteradores:" << endl;
    for (auto it = trie.begin(); it != trie.end(); ++it)
        cout << "  " << *it << endl;

    //! @test contains / startsWith.
    cout << boolalpha;
    cout << "contains(\"car\")   : " << trie.contains("car")   << endl;
    cout << "contains(\"ca\")    : " << trie.contains("ca")    << endl;
    cout << "startsWith(\"ca\")  : " << trie.startsWith("ca")  << endl;
    cout << "startsWith(\"xyz\") : " << trie.startsWith("xyz") << endl;

    //! @test ForEach protegido por mutex.
    cout << "ForEach imprime todas las claves:" << endl;
    using TN = DigitalTrie<TI>::Node;
    trie.ForEach(PrintKey<TN>, cout);

    //! @test FirstThat con predicado.
    auto it = trie.FirstThat(KeyLongerThan<TN>, 3);
    if (it != trie.end())
        cout << "Primera clave con longitud > 3: " << *it << endl;

    //! @test erase con poda.
    cout << "erase(\"card\"): " << trie.erase("card") << endl;
    cout << "erase(\"xxx\") : " << trie.erase("xxx")  << endl;
    cout << "Trie tras erase: " << trie << " (size=" << trie.size() << ")" << endl;

    //! @test Constructor de copia.
    DigitalTrie<TI> copia(trie);
    copia.insert("bird", 7, 70);
    cout << "Copia + bird : " << copia << endl;
    cout << "Original     : " << trie << endl;

    //! @test Constructor de movimiento.
    DigitalTrie<TI> movido = move(copia);
    cout << "Movido       : " << movido << endl;
    cout << "Copia vaciada: " << copia << " (empty=" << copia.empty() << ")" << endl;
}
