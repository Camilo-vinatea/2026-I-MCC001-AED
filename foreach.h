//! @file foreach.h
//! @brief Algoritmos genéricos `ForEach` y `FirstThat` sobre iteradores.
//! @details Funciones plantilla que recorren cualquier par de iteradores
//!          aplicando una función (variádica) a cada elemento. Se usan
//!          desde los contenedores para definir sus métodos públicos de
//!          recorrido.
//! @author Equipo MCC

#ifndef __FOREACH_H__
#define __FOREACH_H__
#include <iostream>
#include <utility> // std::forward

using namespace std;

//! @brief Aplica la función `func` a cada elemento en el rango
//!        `[begin, end)`, reenviando los argumentos extra `args`.
//! @tparam Iterator Tipo de iterador (compatible con ++ y *).
//! @tparam Func     Tipo del callable. Firma esperada: `(Elem&, Args...)`.
//! @tparam Args     Tipos de los argumentos adicionales.
//! @param begin Iterador al primer elemento.
//! @param end   Iterador "uno más allá" del último.
//! @param func  Función/lambda a invocar por cada elemento.
//! @param args  Argumentos adicionales reenviados con `std::forward`.
template <typename Iterator, typename Func, typename... Args>
void ForEach(Iterator begin, Iterator end, Func func, Args &&... args){
    for (auto it = begin; it != end; ++it)
        func(*it, forward<Args>(args)...);
}

//! @brief Recorre el rango y devuelve el primer iterador cuya función
//!        devuelve `true`. Si ninguno coincide, devuelve `end`.
//! @tparam Iterator Tipo de iterador.
//! @tparam Func     Predicado unario (con posibles `Args` extra) que devuelve
//!                   convertible a `bool`.
//! @return Iterador al primer elemento que cumple el predicado, o `end`.
template <typename Iterator, typename Func, typename... Args>
Iterator FirstThat(Iterator begin, Iterator end, Func func, Args &&... args){
    for (auto it = begin; it != end; ++it){
        if(func(*it, forward<Args>(args)...))
            return it;
    }
    return end;
}

//! @brief Sobrecarga que toma un contenedor y delega en la versión por
//!        rango. Requiere que el contenedor tenga `begin()` y `end()`.
//! @tparam Container Tipo de contenedor.
//! @tparam Func      Callable a aplicar.
template <typename Container, typename Func, typename... Args>
void ForEach(Container& v1, Func func, Args &&... args){
    ForEach(v1.begin(), v1.end(), func, forward<Args>(args)...);
}

#endif // __FOREACH_H__
