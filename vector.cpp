#include <iostream>
#include "vector.h"

using namespace std;

template <typename T>
void Print(T &value, ostream& os){
    os << value << ",";
}

template <typename T>
void AddOne(T &value){
    ++value;
}

template <typename T, typename U>
void AddX(T &value, U x){
    value += x;
}

void DemoVector(){
    Vector<TI> v1;
    v1.push_back(7, 15);
    v1.push_back(5, 10);
    v1.push_back(25, 5);
    v1.push_back(10, 20);
    v1.push_back(13, 25);

    cout << "Recorrido con iteradores" << endl;
    v1.ForEach(Print<VectorNode<TI>>, cout);
    cout << endl;
    v1.ForEach(AddOne<VectorNode<TI>>);
    v1.ForEach(Print<VectorNode<TI>>, cout);
    cout << endl;
    v1.ForEach(AddX<VectorNode<TI>, TI>, 10);
    v1.ForEach(Print<VectorNode<TI>>, cout);
    cout << endl;
    cout << "Fin recorrido con iteradores" << endl;

    cout << v1.ToString() << endl;
    cout << "Vector:" << v1 << " despues" << endl;
//  cout.operator<<("Vector:");
//  ==========================
//               cout << v1 
    cout << "Size: " << v1.size() << endl;

    Vector<TD> v2;
    v2.push_back(7.5, 1);
    v2.push_back(5.2, 2);
    v2.push_back(25.1, 3);
    v2.push_back(10.3, 4);
    v2.push_back(13.4, 5);

    cout << v2.ToString() << endl;
    cout << "Size: " << v2.size() << endl;

    Vector<TS> v3;
    v3.push_back("Hello", 5);
    v3.push_back("World", 10);
    v3.push_back("AED", 15);

    cout << v3.ToString() << endl;
    v3.ForEach(AddX<VectorNode<TS>, TS>, "-X");
    cout << v3.ToString() << endl;
    cout << "Size: " << v3.size() << endl;
}