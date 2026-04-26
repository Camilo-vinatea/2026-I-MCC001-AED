#include <iostream>
#include "macros.h"
#include "lists.h"
// #include "vector.h"

using namespace std;

// g++ -std=c++2b main.cpp vector.cpp macros.cpp -o main
// 2011: C++11 => C++14 => C++17 => C++20 => C++23 => C++26

size_t size() { return 0; }
int main() {
    cout << "HelloX World MCS-AED 2026-I" << endl;
    unsigned long i = size()-1;
    signed   long j = size()-1;
    cout << "-1 en un size_t     : " << i << endl;
    cout << "-1 en un signed long: " << j << endl;

    // DemoMacros();
    // DemoVector();
    // DemoConcurrentVector();
    ListsDemo();
    return 0;
}
