#ifndef __MACROS_H__
#define __MACROS_H__

#include "types.h"
// Macros
#define X 5
// x = MULT(x+1, x+2)
#define SUMA(a, b) ((a) + (b))
#define MULT(a, b) ((a) * (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CUADRADO(a)((a) * (a))

#define GetVar(var) c##var

void DemoMacros();

#endif // __MACROS_H__