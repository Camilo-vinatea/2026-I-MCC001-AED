#include <iostream>
#include "vector.h"

using namespace std;

void DemoVector(){
    Vector<TI> v1;
    v1.push_back(7);
    v1.push_back(5);
    v1.push_back(25);
    v1.push_back(10);
    v1.push_back(13);

    for(size_t i = 0; i < v1.size(); ++i){
        cout << v1.get(i) << " ";
    }
    cout << endl;
    cout << "Size: " << v1.size() << endl;

    Vector<TD> v2;
    v2.push_back(7.5);
    v2.push_back(5.2);
    v2.push_back(25.1);
    v2.push_back(10.3);
    v2.push_back(13.4);

    for(size_t i = 0; i < v2.size(); ++i){
        cout << v2.get(i) << " ";
    }
    cout << endl;
    cout << "Size: " << v2.size() << endl;

    Vector<TS> v3;
    v3.push_back("Hello");
    v3.push_back("World");
    v3.push_back("MCS-AED");

    for(size_t i = 0; i < v3.size(); ++i){
        cout << v3.get(i) << " ";
    }
    cout << endl;
    cout << "Size: " << v3.size() << endl;
}