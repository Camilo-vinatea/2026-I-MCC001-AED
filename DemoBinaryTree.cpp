#include <iostream>
#include <fstream>
#include "containers/binarytree.h"

using namespace std;

void DemoBinaryTree() {
    cout << "\n=== DemoBinaryTree ===" << endl;

    // --- Construcción BST ascendente ---
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

    auto imprimirNodo = [](auto& nodo) {
        cout << nodo.getData() << " ";
    };

    cout << "\nInorden Adelante  (LNR): ";
    bt.ForEach(imprimirNodo);
    cout << endl;

    cout << "Inorden Reverso   (RNL): ";
    bt.ReverseForEach(imprimirNodo);
    cout << endl;

    cout << "Preorden Adelante  (NLR): ";
    bt.ForEachPreorder(imprimirNodo);
    cout << endl;

    cout << "Preorden Reverso   (NRL): ";
    bt.ForEachReversePreorder(imprimirNodo);
    cout << endl;

    cout << "Postorden Adelante (LRN): ";
    bt.ForEachPostorder(imprimirNodo);
    cout << endl;

    cout << "Postorden Reverso  (RLN): ";
    bt.ForEachReversePostorder(imprimirNodo);
    cout << endl;

    // --- Búsqueda condicional ---
    auto encontrado = bt.FirstThat([](auto& nodo) {
        return nodo.getData() > 35;
    });
    if (encontrado != bt.end())
        cout << "\nPrimero con dato > 35: " << (*encontrado).getData() << endl;

    // --- Copy constructor ---
    BinaryTree<BinaryTreeTraits<TI>> bt2(bt);
    cout << "\nCopia (inorden): ";
    bt2.ForEach(imprimirNodo);
    cout << endl;

    // --- Move constructor ---
    BinaryTree<BinaryTreeTraits<TI>> bt3(move(bt2));
    cout << "Movimiento (inorden, origen vacio=" << bt2.empty() << "): ";
    bt3.ForEach(imprimirNodo);
    cout << endl;

    // --- Escritura en disco ---
    {
        ofstream archivo("binarytree.txt");
        archivo << bt;
        cout << "\nEscrito en binarytree.txt" << endl;
    }

    // --- Lectura desde archivo ---
    {
        BinaryTree<BinaryTreeTraits<TI>> bt4;
        ifstream archivo("binarytree.txt");
        archivo >> bt4;
        cout << "Leido desde disco (inorden): ";
        bt4.ForEach(imprimirNodo);
        cout << endl;
    }

    // --- Árbol descendente ---
    BinaryTree<BinaryTreeTraits<TI, greater<TI>>> btDesc;
    btDesc.insert(50, 0);
    btDesc.insert(30, 1);
    btDesc.insert(70, 2);
    btDesc.insert(20, 3);
    btDesc.insert(40, 4);
    cout << "\nBST descendente (inorden): ";
    btDesc.ForEach(imprimirNodo);
    cout << endl;

}
