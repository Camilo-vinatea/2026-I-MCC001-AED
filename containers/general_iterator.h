//! @file general_iterator.h
//! @brief Iterador genérico base (CRTP) usado por todos los contenedores.
//! @details Implementa el patrón CRTP: la clase derivada pasa su propio
//!          tipo como `IteratorBase` y eso le permite especializar
//!          `operator++` sin perder una API uniforme.
//!          Provee: constructores por copia/movimiento, asignación,
//!          comparación `==`/`!=`, desreferencia `*` y `->`, y acceso
//!          al nodo (`getNode()`).
//! @author Equipo MCC

#ifndef __ITERATOR_H__
#define __ITERATOR_H__
#include <algorithm>
#include <utility>
using namespace std;

//! @class general_iterator
//! @brief Plantilla CRTP para iteradores uniformes en todos los contenedores.
//! @tparam Container    Tipo del contenedor dueño del nodo.
//! @tparam IteratorBase Tipo de la clase derivada (CRTP).
template <typename Container, class IteratorBase>
class general_iterator
{
public:
    //! @typedef Node
    //! @brief Tipo de nodo, tomado del contenedor.
    using Node   = typename Container::Node;
    //! @typedef Myself
    //! @brief Alias de la propia clase base.
    using Myself = general_iterator<Container, IteratorBase>;

protected:
    Container *m_pContainer; //!< Puntero al contenedor dueño.
    Node      *m_pNode;      //!< Puntero al nodo actual.

public:
    //! @brief Constructor principal.
    //! @param pContainer Contenedor asociado.
    //! @param pNode      Nodo inicial al que apunta el iterador.
    general_iterator(Container *pContainer, Node *pNode)
        : m_pContainer(pContainer), m_pNode(pNode) {}

    //! @brief Constructor de copia.
    general_iterator(Myself &other)
        : m_pContainer(other.m_pContainer), m_pNode(other.m_pNode) {}

    //! @brief Constructor de movimiento.
    general_iterator(Myself &&other)
    {
        m_pContainer = move(other.m_pContainer);
        m_pNode      = move(other.m_pNode);
    }

    //! @brief Operador de asignación: copia/mueve desde otra instancia
    //!        derivada. Devuelve la referencia a `IteratorBase`.
    IteratorBase operator=(IteratorBase &iter)
    {
        m_pContainer = move(iter.m_pContainer);
        m_pNode      = move(iter.m_pNode);
        return *(IteratorBase *)this;
    }

    //! @brief Devuelve el puntero al nodo actual.
    Node *getNode() const { return m_pNode; }

    //! @brief Compara dos iteradores por la dirección del nodo.
    friend bool operator==(const IteratorBase &a, const IteratorBase &b)
    { return a.getNode() == b.getNode(); }

    //! @brief Negación de la igualdad.
    friend bool operator!=(const IteratorBase &a, const IteratorBase &b)
    { return !(a == b); }

    //! @brief Desreferencia (referencia mutable al nodo).
    Node &operator*()        { return *m_pNode; }
    //! @brief Acceso a miembro del nodo a través del iterador.
    Node* operator->() const { return m_pNode; }
};

#endif
