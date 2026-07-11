//! @file basetrait.h
//! @brief Traits base (politics para comparadores y contenedor genérico).
//! @details Define dos familias de plantillas:
//!          - `BaseContainerTrait<T, Node>`: asocia un `value_type` y un
//!            `Node` a un contenedor.
//!          - `AscendingTrait<T>` / `DescendingTrait<T>`: exponen el
//!            functor comparador `less<T>` o `greater<T>` que usarán los
//!            contenedores ordenados.
//! @author Equipo MCC

#ifndef __BASE_TRAIT_H__
#define __BASE_TRAIT_H__
#include <functional> // less, greater

using namespace std;

//! @struct BaseContainerTrait
//! @brief Trait base que mapea un valor `T` a su tipo de `Node`.
//! @tparam _T    Tipo de dato almacenado.
//! @tparam _Node Tipo de nodo concreto del contenedor.
template <typename _T, typename _Node>
struct BaseContainerTrait{
    using value_type = _T;     //!< Tipo de valor almacenado.
    using Node       = _Node;  //!< Tipo del nodo contenedor.
};

//! @struct AscendingTrait
//! @brief Trait que aporta el comparador ascendente (`std::less<T>`).
template <typename _T>
struct AscendingTrait{
    using Comp = less<_T>;     //!< Comparador ascendente.
};

//! @struct DescendingTrait
//! @brief Trait que aporta el comparador descendente (`std::greater<T>`).
template <typename _T>
struct DescendingTrait{
    using Comp = greater<_T>;  //!< Comparador descendente.
};

#endif // __BASE_TRAIT_H__
