#include <iostream>
#include "macros.h"

using namespace std;

void DemoMacros(){
    cout << "DemoMacros" << endl;
    cout << "X = " << X << endl;
    cout << "SUMA(2, 3) = " << SUMA(2, 3) << endl;
    cout << "SUMA(X, 3) = " << SUMA(X, 3) << endl;
    cout << "SUMA(X, X) = " << SUMA(X, X) << endl;
    cout << "SUMA(X*2, X+3) = " << SUMA(X*2, X+3) << endl;

    cout << "MULT(X, X) = " << MULT(X, X) << endl;
    cout << "MULT(X*2, X+3) = " << MULT(X*2, X+3) << endl;

    cout << "MAX(X, 3) = " << MAX(X, 3) << endl;
    cout << "MAX(X, X) = " << MAX(X, X) << endl;
    cout << "MAX(X*2, X+3) = " << MAX(X*2, X+3) << endl;

    cout << "CUADRADO(X) = " << CUADRADO(X) << endl;
    cout << "CUADRADO(X*2) = " << CUADRADO(X*2) << endl;

    int c3 = 10, c4 = 20, c5 = 30;
    cout << "GetVar(c3) = " << GetVar(3) << endl;
    cout << "GetVar(c4) = " << GetVar(4) << endl;
    cout << "GetVar(c5) = " << GetVar(5) << endl;
    // cout << "GetVar(c10) = " << GetVar(10) << endl;

    // for(int i = 3 ; i <= 5 ; ++i)
    //     cout << "GetVar(c" << i << ") = " << GetVar(i) << endl;
    cout << "Estamos en la linea " << __LINE__ << " del archivo " << __FILE__ << endl;
    cout << "Hora: "       << __TIME__ << endl;
    cout << "Fecha: "      << __DATE__ << endl;
    cout << "Version: "    << __VERSION__ << endl;
    cout << "Compilador: " << __cplusplus << endl;
}