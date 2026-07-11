//! @file vector.h
//! @brief Contenedor `Vector<Traits>` (almacenamiento contiguo dinámico).
//! @details Vector propio con iteradores forward/backward, `ForEach`,
//!          `FirstThat`, `push_back`, `ToString` y protección por mutex.
//!          El nodo se modela con la estructura `VectorNode` (un valor y
//!          un `Ref`).
//! @author Equipo MCC

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cstddef>
#include <sstream>
#include <mutex>
#include "general_iterator.h"
#include "../types.h"
#include "../foreach.h"
#include "basetrait.h"

//! @class vector_forward_iterator
//! @brief Iterador hacia adelante para `Vector` (incrementa el puntero).
template <typename Container>
class vector_forward_iterator
    : public general_iterator<Container, vector_forward_iterator<Container>>
{
public:
    using MySelf = vector_forward_iterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    //! @brief Avanza al siguiente elemento (aritmética de punteros).
    MySelf operator++() { this->m_pNode++; return *this; }
};

//! @class vector_backward_iterator
//! @brief Iterador hacia atrás para `Vector` (decrementa el puntero).
template <typename Container>
class vector_backward_iterator
    : public general_iterator<Container, vector_backward_iterator<Container>>
{
public:
    using MySelf = vector_backward_iterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
    //! @brief Retrocede al elemento anterior.
    MySelf operator++() { this->m_pNode--; return *this; }
};

//! @struct VectorNode
//! @brief Elemento almacenado: par `(value, Ref)` con operaciones básicas.
//! @tparam Traits Trait del `Vector` (define `value_type`).
template <typename Traits>
struct VectorNode{
    using value_type = typename Traits::value_type;

    value_type   m_data;           //!< Valor almacenado.
    Ref          m_ref;            //!< Identificador asociado.

    //! @brief Constructor por defecto.
    VectorNode() : m_data(value_type()), m_ref(Ref()) {}
    //! @brief Constructor con valor y referencia.
    VectorNode(value_type data, Ref ref) : m_data(data), m_ref(ref) {}

    //! @brief Devuelve representación textual `(data,ref)`.
    string ToString(){
        ostringstream oss;
        oss << "(" << m_data << "," << m_ref << ")";
        return oss.str();
    }
    //! @brief Acceso de solo lectura al dato.
    value_type  GetData() const { return m_data; }
    //! @brief Acceso por referencia al dato.
    value_type& GetDataRef()    { return m_data; }
    //! @brief Acceso de solo lectura al `Ref`.
    Ref GetRef() const { return m_ref; }
    //! @brief Incrementa el dato en una unidad.
    void operator++() { ++m_data; }
    //! @brief Suma al dato.
    void operator+=(const value_type& value) { m_data += value; }
};

//! @struct VectorTraits
//! @brief Trait listo-para-usar que combina `BaseContainerTrait` y un nodo.
template <typename T>
struct VectorTraits : public BaseContainerTrait<T, VectorNode<T>>{};

//! @brief Operador de salida por stream (delega en `ToString`).
template <typename Traits>
ostream& operator<<(ostream& os, VectorNode<Traits>& vn){
    return os << vn.ToString();
}

//! @class Vector
//! @brief Vector dinámico genérico con iteradores y recorrido concurrente.
//! @tparam Traits Define `value_type`, `Node`, etc.
template <typename Traits>
class Vector{
public:
    using value_type        = typename Traits::value_type;
    using Node              = typename Traits::Node;
    using MySelf            = Vector<Traits>;
    using forward_iterator  = vector_forward_iterator<MySelf>;
    using backward_iterator = vector_backward_iterator<MySelf>;

private:
    Node  *m_data;       //!< Arreglo dinámico.
    size_t m_size;       //!< Número de elementos usados.
    size_t m_capacity;   //!< Capacidad total reservada.
    mutex  m_mtx;        //!< Mutex para operaciones concurrentes.

private:
    //! @brief Duplica la capacidad y copia los elementos.
    void resize();

public:
    //! @brief Construye un vector con la capacidad inicial indicada.
    //! @param capacity Capacidad (mínimo 10 en el primer `resize`).
    Vector(size_t capacity = 10);
    //! @brief Destructor; libera la memoria reservada.
    ~Vector();

    //! @brief Inserta al final; si no hay espacio, llama a `resize()`.
    //! @param value Valor a insertar.
    //! @param ref   Identificador asociado.
    void push_back(value_type value, Ref ref);

    //! @brief Número actual de elementos.
    size_t size();
    //! @brief Representación textual estilo `[a,b,c]`.
    string ToString();

    //! @brief Iterador al primer elemento.
    forward_iterator  begin()  { return forward_iterator(this, m_data); }
    //! @brief Iterador "uno más allá" del último.
    forward_iterator  end()    { return forward_iterator(this, m_data + m_size); }
    //! @brief Iterador inverso al último elemento.
    backward_iterator rbegin() { return backward_iterator(this, m_data + m_size - 1); }
    //! @brief Iterador inverso "uno más allá" del principio.
    backward_iterator rend()   { return backward_iterator(this, m_data - 1); }

    //! @brief Recorre el vector hacia adelante aplicando `func(elem, args...)`.
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&... args){
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    //! @brief Recorre el vector hacia atrás aplicando `func(elem, args...)`.
    template <typename Func, typename... Args>
    void ReverseForEach(Func func, Args &&... args){
        ::ForEach(rbegin(), rend(), func, forward<Args>(args)...);
    }

    //! @brief Primer elemento que cumple `func(elem, args...)` (forward).
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

    //! @brief Primer elemento que cumple (recorrido inverso).
    template <typename Func, typename... Args>
    backward_iterator ReverseFirstThat(Func func, Args &&... args){
        return ::FirstThat(rbegin(), rend(), func, forward<Args>(args)...);
    }
};

//! @brief Constructor: reserva el bloque inicial.
template <typename Traits>
Vector<Traits>::Vector(size_t capacity) {
    m_data = nullptr;
    m_size = 0;
    m_capacity = capacity;
    m_data = new Node[m_capacity];
}

//! @brief Destructor: libera la memoria del arreglo.
template <typename Traits>
Vector<Traits>::~Vector() {
    delete[] m_data;
}

//! @brief Inserta al final con mutex; redimensiona si es necesario.
template <typename Traits>
void Vector<Traits>::push_back(value_type value, Ref ref) {
    if (m_size == m_capacity)
        resize();
    scoped_lock<mutex> lock(m_mtx);
    m_data[m_size] = Node(value, ref);
    m_size++;
}

//! @brief Duplica la capacidad (mínimo 10) y copia los datos.
template <typename Traits>
void Vector<Traits>::resize() {
    scoped_lock<mutex> lock(m_mtx);
    m_capacity = m_capacity < 10 ? 10 : m_capacity * 2;
    Node *newData = new Node[m_capacity];
    for (size_t i = 0; i < m_size; i++)
        newData[i] = m_data[i];
    delete[] m_data;
    m_data = newData;
}

//! @brief Devuelve el tamaño actual.
template <typename Traits>
size_t Vector<Traits>::size() {
    return m_size;
}

//! @brief Devuelve un string con el formato `[a,b,c]`.
template <typename Traits>
string Vector<Traits>::ToString() {
    scoped_lock lock(m_mtx);
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < m_size-1; i++)
        oss << m_data[i] << ",";
    if (m_size > 0)
        oss << m_data[m_size-1];
    oss << "]";
    return oss.str();
}

//! @brief Operador `<<` para imprimir el vector.
template <typename Traits>
ostream& operator<<(ostream& os, Vector<Traits>& v){
    return os << v.ToString();
}

//! @brief Demo del Vector y operaciones básicas.
void DemoVector();
//! @brief Demo de operaciones concurrentes sobre `Vector` (múltiples hilos).
void DemoConcurrentVector();

#endif // __VECTOR_H__
