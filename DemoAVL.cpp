#include <iostream>
#include <cassert>
#include "containers/avl.h"

using namespace std;

void DemoAVL() {
    cout << "\n=== DemoAVL ===" << endl;

    // Insertar 1..7 en orden ascendente — caso degenerado para BST puro (altura=7)
    // AVL debe rebalancear a altura=3 (árbol perfecto para 7 nodos)
    AVL<BinaryTreeTraits<TI>> avl;
    for (int i = 1; i <= 7; ++i)
        avl.insert(i, (Ref)(i - 1));

    // --- Verificar tamaño ---
    assert(avl.size() == 7);
    cout << "OK: size == 7" << endl;

    // --- Verificar propiedad BST: inorden ascendente ---
    cout << "Inorden: ";
    TI prev = 0;
    bool sorted = true;
    for (auto& nodo : avl.inorder()) {
        cout << nodo.getData() << " ";
        if (nodo.getData() <= prev) sorted = false;
        prev = nodo.getData();
    }
    cout << endl;
    assert(sorted);
    cout << "OK: inorden ascendente" << endl;

    // --- Verificar balance: altura raiz == 3 ---
    // 7 nodos balanceados perfectamente => floor(log2(7))+1 = 3
    size_t h = avl.tree_height();
    cout << "Altura raiz: " << h << endl;
    assert(h == 3);
    cout << "OK: altura == 3 (balanceado)" << endl;

    // --- Iteradores heredados funcionan sobre AVL ---
    cout << "Preorden (NLR): ";
    for (auto& nodo : avl.preorder()) cout << nodo.getData() << " ";
    cout << endl;

    cout << "Postorden (LRN): ";
    for (auto& nodo : avl.postorder()) cout << nodo.getData() << " ";
    cout << endl;

    // --- AVL descendente ---
    AVL<BinaryTreeTraits<TI, DescendingTrait<TI>>> avlDesc;
    for (int i = 1; i <= 5; ++i)
        avlDesc.insert(i, (Ref)(i - 1));
    cout << "Descendente inorden (5..1): ";
    for (auto& nodo : avlDesc.inorder()) cout << nodo.getData() << " ";
    cout << endl;
    assert(avlDesc.size() == 5);
    cout << "OK: AVL descendente size == 5" << endl;

    cout << "=== DemoAVL OK ===" << endl;
}
