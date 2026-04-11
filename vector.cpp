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

    cout << v1.ToString() << endl;
    cout << v1 << endl;
    cout << "Size: " << v1.size() << endl;

    Vector<TD> v2;
    v2.push_back(7.5);
    v2.push_back(5.2);
    v2.push_back(25.1);
    v2.push_back(10.3);
    v2.push_back(13.4);

    cout << v2.ToString() << endl;
    cout << "Size: " << v2.size() << endl;

    Vector<TS> v3;
    v3.push_back("Hello");
    v3.push_back("World");
    v3.push_back("MCS-AED");

    cout << v3.ToString() << endl;
    cout << "Size: " << v3.size() << endl;
}