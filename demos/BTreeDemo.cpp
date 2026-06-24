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
        BTree<BTreeTraits<char>> bt(BTreeSize);
        for (i = 0; keys1[i]; i++){
               //cout<<"Inserting "<<keys1[i]<<endl;
               bt.Insert(keys1[i], i*i);
        }
        bt.Print(cout);
}
