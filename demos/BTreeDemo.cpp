//#include <iostream.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "../containers/BTree.h"

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const int BTreeSize = 3;

void BTreeDemo(){
        int i;
        using Node = BTree<BTreeTraits<char>>::Node;

        BTree<BTreeTraits<char>> bt(BTreeSize);
        for (i = 0; keys1[i]; i++){
               //cout<<"Inserting "<<keys1[i]<<endl;
               bt.Insert(keys1[i], i*i);
        }
        bt.Print(cout);

        // 1) Recorrido con el método propio de BTree.h (indica a qué
        //    profundidad (level) está cada clave). No usa iteradores.
        cout << "\n1) Metodo interno del arbol, con nivel de profundidad:\n";
        bt.ForEachInternal([](Node &n, tree_height_t level){
               cout << string(level, '\t') << n.key << "->" << n.ObjID << "\n";
        });

        // 2) Recorrido de menor a mayor usando el iterador hacia adelante.
        //    "for (auto &n : bt)" funciona porque BTree ofrece begin()/end().
        cout << "\n2) Iterador hacia adelante (menor a mayor), con range-for:\n";
        for (auto &n : bt)
               cout << n.key << " ";
        cout << "\n";

        // 3) Recorrido de mayor a menor usando el iterador hacia atras.
        //    No existe range-for para esto en C++, hay que escribir el loop
        //    a mano usando rbegin()/rend() en vez de begin()/end().
        cout << "\n3) Iterador hacia atras (mayor a menor):\n";
        for (auto it = bt.rbegin(); it != bt.rend(); ++it)
               cout << it->key << " ";
        cout << "\n";

        // 4) Mismo recorrido que el punto 2, pero usando ::ForEach (externo)
        cout << "\n4) Funcion generica ForEach() de foreach.h, de menor a mayor:\n";
        ::ForEach(bt.begin(), bt.end(), [](Node &n){ cout << n.key << " "; });
        cout << "\n";

        // 5) Mismo recorrido que el punto 3, pero usando ::ForEach (externo)
        //    Se le pasa rbegin()/rend() para que vaya al revés.
        cout << "\n5) Funcion generica ForEach() de foreach.h, de mayor a menor:\n";
        ::ForEach(bt.rbegin(), bt.rend(), [](Node &n){ cout << n.key << " "; });
        cout << "\n";
}
