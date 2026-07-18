#include <iostream>
#include "macros.h"
#include "lists.h"
// #include "containers/vector.h"
#include "containers/binarytree.h"
#include "containers/avl.h"
#include "containers/Btree.h"
#include "containers/mapDemo.h"
#include "containers/digitalTrie.h"
#include "containers/hashPoissonDemo.h"
#include "containers/dijkstraDemo.h"
#include "containers/lruCacheDemo.h"

using namespace std;

// g++ -std=c++2b main.cpp vector.cpp macros.cpp -o main
// 2011: C++11 => C++14 => C++17 => C++20 => C++23 => C++26

size_t size1() { return 0; }
unsigned char size2() { return 0; }
int main() {
    // DemoMacros();
    // DemoVector();
    // DemoConcurrentVector();
    // ListsDemo();
    // DemoBinaryTree();
    // DemoAVL();
    // BTreeDemo();
    // stdmapDemo();
     DigitalTrieDemo();
    // HashPoissonDemo();
    //DijkstraDemo();
    //cout << "\n";
    //LRUCacheDemo();
    return 0;
}
