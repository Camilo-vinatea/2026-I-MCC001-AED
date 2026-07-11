//! @file DemoBinaryTree.cpp
//! @brief Demo del BST: inserciones, 6 recorridos, copia/movimiento,
//!        persistencia y comparador descendente.
//! @author Equipo MCC

#include <iostream>
#include <fstream>
#include "../containers/binarytree.h"

using namespace std;

//! @brief Demo exhaustiva del BinaryTree:
//!  - Construcción BST ascendente (50,30,70,20,40,60,80).
//!  - 6 recorridos (in/pre/postorder, forward + backward).
//!  - `FirstThat`: primer nodo con dato > 35.
//!  - Constructor de copia y movimiento.
//!  - Persistencia: escribe/lee desde "binarytree.txt".
//!  - Variante con `DescendingTrait<TI>`.
//! @post "binarytree.txt" se crea en el directorio actual.
void DemoBinaryTree() {
    cout << "\n=== DemoBinaryTree ===" << endl;

    //! @test Construcción BST ascendente.
    BinaryTree<BinaryTreeTraits<TI>> bt;
    bt.insert(50, 0);
    bt.insert(30, 1);
    bt.insert(70, 2);
    bt.insert(20, 3);
    bt.insert(40, 4);
    bt.insert(60, 5);
    bt.insert(80, 6);

    cout << "Tamanio: " << bt.size() << endl;
    cout << "toString (inorden): " << bt.toString() << endl;

    //! @brief Lambda usado para imprimir cualquier nodo.
    auto imprimirNodo = [](auto& nodo) {
        cout << nodo.getData() << " ";
    };

    //! @test In-order, reverse-in-order, pre-order, reverse-pre-order,
    //!       post-order, reverse-post-order.
    cout << "\nInorden Adelante  (LNR): ";
    for (auto& nodo : bt.inorder())           cout << nodo.getData() << " ";
    cout << endl;

    cout << "Inorden Reverso   (RNL): ";
    for (auto& nodo : bt.reverse_inorder())   cout << nodo.getData() << " ";
    cout << endl;

    cout << "Preorden Adelante  (NLR): ";
    for (auto& nodo : bt.preorder())          cout << nodo.getData() << " ";
    cout << endl;

    cout << "Preorden Reverso   (NRL): ";
    for (auto& nodo : bt.reverse_preorder())  cout << nodo.getData() << " ";
    cout << endl;

    cout << "Postorden Adelante (LRN): ";
    for (auto& nodo : bt.postorder())         cout << nodo.getData() << " ";
    cout << endl;

    cout << "Postorden Reverso  (RLN): ";
    for (auto& nodo : bt.reverse_postorder()) cout << nodo.getData() << " ";
    cout << endl;

    //! @test Búsqueda condicional: primer nodo con `dato > 35`.
    auto encontrado = bt.FirstThat([](auto& nodo) {
        return nodo.getData() > 35;
    });
    if (encontrado != bt.end())
        cout << "\nPrimero con dato > 35: " << (*encontrado).getData() << endl;

    //! @test Constructor de copia.
    BinaryTree<BinaryTreeTraits<TI>> bt2(bt);
    cout << "\nCopia (inorden): ";
    bt2.ForEach(imprimirNodo);
    cout << endl;

    //! @test Constructor de movimiento.
    BinaryTree<BinaryTreeTraits<TI>> bt3(move(bt2));
    cout << "Movimiento (inorden, origen vacio=" << bt2.empty() << "): ";
    bt3.ForEach(imprimirNodo);
    cout << endl;

    //! @test Escritura en "binarytree.txt" con `operator<<`.
    {
        ofstream archivo("binarytree.txt");
        archivo << bt;
        cout << "\nEscrito en binarytree.txt" << endl;
    }

    //! @test Lectura desde disco con `operator>>`.
    {
        BinaryTree<BinaryTreeTraits<TI>> bt4;
        ifstream archivo("binarytree.txt");
        archivo >> bt4;
        cout << "Leido desde disco (inorden): ";
        bt4.ForEach(imprimirNodo);
        cout << endl;
    }

    //! @test BST descendente (`DescendingTrait`).
    BinaryTree<BinaryTreeTraits<TI, DescendingTrait<TI>>> btDesc;
    btDesc.insert(50, 0);
    btDesc.insert(30, 1);
    btDesc.insert(70, 2);
    btDesc.insert(20, 3);
    btDesc.insert(40, 4);
    cout << "\nBST descendente (inorden): ";
    btDesc.ForEach(imprimirNodo);
    cout << endl;
}
