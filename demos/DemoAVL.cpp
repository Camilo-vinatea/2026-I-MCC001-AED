#include <iostream>
#include <cassert>
#include "../containers/avl.h"

using namespace std;

// Imprime una linea separadora
static void separador(char c = '-', int n = 40) {
    cout << string(n, c) << "\n";
}

void DemoAVL() {
    separador('=');
    cout << "  DEMO AVL - Arbol Binario Balanceado\n";
    separador('=');

    // ----------------------------------------------------------------
    // Parte 1: insercion paso a paso 1..7 en orden ascendente
    // Un BST puro degeneraria en lista (altura 7).
    // El AVL rebalancea automaticamente via rotaciones.
    // ----------------------------------------------------------------
    cout << "\n[1] Insercion 1..7 en orden ascendente\n";
    cout << "    (caso degenerado para BST puro => altura 7)\n";
    cout << "    El AVL debe mantener altura 3 (log2(7)+1)\n\n";
    cout << "  Vista: derecha=arriba, izquierda=abajo, cada nivel = 4 espacios\n\n";

    AVL<BinaryTreeTraits<TI>> avl;
    for (int i = 1; i <= 7; ++i) {
        avl.insert(i, (Ref)(i - 1));
        cout << "  -- Despues de insertar " << i << " --\n";
        avl.print_tree();
        cout << "  Altura raiz: " << avl.tree_height()
             << "  |  Nodos: " << avl.size() << "\n\n";
    }

    // ----------------------------------------------------------------
    // Parte 2: verificaciones de correctitud
    // ----------------------------------------------------------------
    separador();
    cout << "[2] Verificaciones\n\n";

    assert(avl.size() == 7);
    cout << "  Tamanio: " << avl.size() << "  OK\n";

    size_t h = avl.tree_height();
    assert(h == 3);
    cout << "  Altura:  " << h << "  OK (optimo para 7 nodos)\n";

    cout << "  Inorden (LNR, debe ser 1..7 ordenado): ";
    TI prev = 0; bool sorted = true;
    for (auto& nodo : avl.inorder()) {
        cout << nodo.getData() << " ";
        if (nodo.getData() <= prev) sorted = false;
        prev = nodo.getData();
    }
    cout << "\n";
    assert(sorted);
    cout << "  Propiedad BST: OK\n";

    // ----------------------------------------------------------------
    // Parte 3: recorridos heredados de BinaryTree
    // ----------------------------------------------------------------
    separador();
    cout << "[3] Recorridos heredados de BinaryTree\n\n";

    cout << "  Inorden   (LNR): ";
    for (auto& n : avl.inorder())          cout << n.getData() << " ";
    cout << "\n";

    // ::ForEach(avl.inorder().begin(), avl.inorder().end(), [](Node& n){cout << n.getData() << " ";});

    cout << "  Preorden  (NLR): ";
    for (auto& n : avl.preorder())         cout << n.getData() << " ";
    cout << "\n";

    cout << "  Postorden (LRN): ";
    for (auto& n : avl.postorder())        cout << n.getData() << " ";
    cout << "\n";

    cout << "  Rev.Inorden    : ";
    for (auto& n : avl.reverse_inorder())  cout << n.getData() << " ";
    cout << "\n";

    // ----------------------------------------------------------------
    // Parte 4: AVL descendente
    // ----------------------------------------------------------------
    separador();
    cout << "[4] AVL con comparador Descendente (1..5)\n\n";

    AVL<BinaryTreeTraits<TI, DescendingTrait<TI>>> avlDesc;
    for (int i = 1; i <= 5; ++i)
        avlDesc.insert(i, (Ref)(i - 1));

    cout << "  Arbol (valores mayores a la izquierda):\n";
    avlDesc.print_tree();
    cout << "\n";

    cout << "  Inorden descendente: ";
    for (auto& n : avlDesc.inorder()) cout << n.getData() << " ";
    cout << "\n";
    assert(avlDesc.size() == 5);
    cout << "  Tamanio: " << avlDesc.size() << "  OK\n";

    separador('=');
    cout << "  DEMO AVL - Todas las verificaciones OK\n";
    separador('=');
    cout << "\n";
}
