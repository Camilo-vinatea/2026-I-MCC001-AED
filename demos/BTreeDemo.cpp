//! @file BTreeDemo.cpp
//! @brief Demostración del B-Tree: insertar, imprimir y recorrer.
//! @details Inserta ~62 claves de un set, luego imprime:
//!          1. Recorrido interno con nivel (sangrado).
//!          2. Iterador forward con `range-for`.
//!          3. Iterador backward con loop manual.
//!          4. ::ForEach externo (forward).
//!          5. ::ForEach externo (backward).
//!          6. `BTree::ForEach` (interno) con mutex sostenido.
//! @author Equipo MCC

#include <iostream>
#include "../containers/BTree.h"

using namespace std;

//! @brief Set de claves para insertar (orden aleatorio, todas distintas).
const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
//! @brief Set ascendente `0..9A..Za..z` (peor caso para el árbol).
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
//! @brief Set aleatorio adicional.
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

//! @brief Orden con el que se construye el B-Tree de demo.
const int BTreeSize = 3;

//! @brief Función de demo completa del B-Tree.
//!
//! Salidas:
//! - "1) Metodo interno del arbol, con nivel de profundidad"
//! - "2) Iterador hacia adelante (menor a mayor), con range-for"
//! - "3) Iterador hacia atras (mayor a menor)"
//! - "4) Funcion generica ForEach() de foreach.h, de menor a mayor"
//! - "5) Funcion generica ForEach() de foreach.h, de mayor a menor"
//! - "6) Metodo ForEach de BTree (usa ::ForEach con mutex sostenido)"
void BTreeDemo(){
    int i;
    using Node = BTree<BTreeTraits<char>>::Node;

    //! @test Construye un B-Tree<char> de orden 3.
    BTree<BTreeTraits<char>> bt(BTreeSize);
    for (i = 0; keys1[i]; i++)
        bt.Insert(keys1[i], i*i);
    bt.Print(cout);

    //! @test 1) Recorrido interno con `level`.
    cout << "\n1) Metodo interno del arbol, con nivel de profundidad:\n";
    bt.ForEachInternal([](Node &n, tree_height_t level){
        cout << string(level, '\t') << n.key << "->" << n.ObjID << "\n";
    });

    //! @test 2) Iterador forward + range-for.
    cout << "\n2) Iterador hacia adelante (menor a mayor), con range-for:\n";
    for (auto &n : bt)
        cout << n.key << " ";
    cout << "\n";

    //! @test 3) Iterador backward con loop manual.
    cout << "\n3) Iterador hacia atras (mayor a menor):\n";
    for (auto it = bt.rbegin(); it != bt.rend(); ++it)
        cout << it->key << " ";
    cout << "\n";

    //! @test 4) ::ForEach externo (forward).
    cout << "\n4) Funcion generica ForEach() de foreach.h, de menor a mayor:\n";
    ::ForEach(bt.begin(), bt.end(), [](Node &n){ cout << n.key << " "; });
    cout << "\n";

    //! @test 5) ::ForEach externo (backward).
    cout << "\n5) Funcion generica ForEach() de foreach.h, de mayor a menor:\n";
    ::ForEach(bt.rbegin(), bt.rend(), [](Node &n){ cout << n.key << " "; });
    cout << "\n";

    //! @test 6) BTree::ForEach (interno, con mutex).
    cout << "\n6) Metodo ForEach de BTree (usa ::ForEach con mutex sostenido):\n";
    bt.ForEach([](Node &n){ cout << n.key << " "; });
    cout << "\n";
}
