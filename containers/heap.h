//! @file heap.h
//! @brief Implementación de un Heap (min-heap/max-heap) sobre `std::vector`.
//! @details Definidos en este header:
//!          - `HeapNode<Traits>`: nodo con `data` y `Ref`.
//!          - `AscendingHeapTrait<T>` y `DescendingHeapTrait<T>`: traits
//!            que seleccionan el comparador.
//!          - `Heap<Traits>`: contenedor que ofrece `insert`, `extract`,
//!            `peek_min`, `empty`, `size` y reorganización interna con
//!            `heapify_up` / `heapify_down`.
//! @author Equipo MCC

#ifndef __HEAP_H__
#define __HEAP_H__
#include <vector>
#include <mutex>
#include "vector.h"

//! @class HeapNode
//! @brief Nodo almacenado en el `Heap`. Contiene un valor y un `Ref`.
//! @tparam Traits Trait que expone `value_type`.
template <typename Traits>
class HeapNode{
private:
    typename Traits::value_type m_data; //!< Valor almacenado.
    Ref m_ref;                          //!< Identificador asociado.
public:
    //! @brief Constructor.
    //! @param data Valor inicial.
    //! @param ref  Referencia inicial.
    HeapNode(typename Traits::value_type data, Ref ref)
        : m_data(data), m_ref(ref) {}

    //! @brief Devuelve el dato por valor.
    typename Traits::value_type GetData() const { return m_data; }
    //! @brief Devuelve el `Ref` asociado.
    Ref GetRef() const { return m_ref; }
};

//! @struct AscendingHeapTrait
//! @brief Trait para heap ascendente (min-heap en el tope).
template <typename T>
struct AscendingHeapTrait
    : public BaseContainerTrait<T, HeapNode<T> >,
      public AscendingTrait<T>
{};

//! @struct DescendingHeapTrait
//! @brief Trait para heap descendente (max-heap en el tope).
template <typename T>
struct DescendingHeapTrait
    : public BaseContainerTrait<T, HeapNode<T> >,
      public DescendingTrait<T>
{};

//! @class Heap
//! @brief Montículo binario. Por defecto min-heap con `AscendingHeapTrait`.
//! @tparam Traits Debe exponer `value_type`, `Node` y `Comp`.
//! @note Consultar: https://www.cs.usfca.edu/~galles/visualization/Heap.html
//!       La raíz está en la posición `[0]`.
template <typename Traits>
class Heap {
public:
    using value_type = typename Traits::value_type;  //!< Tipo del valor.
    using Node       = typename Traits::Node;        //!< Tipo del nodo.
    using Comp       = typename Traits::Comp;        //!< Comparador.
    using MySelf     = Heap<Traits>;                 //!< Alias propio.

private:
    vector<Node> m_heap;   //!< Almacenamiento contiguo.
    Comp         m_comp;   //!< Comparador usado por heapify.

public:
    //! @brief Inserta un valor y lo reubica hacia arriba (`heapify_up`).
    //! @param value Valor a insertar.
    //! @param ref   Identificador asociado.
    void insert(const value_type &value, Ref ref) {
        m_heap.push_back( Node(value, ref) );
        heapify_up(m_heap.size() - 1);
    }

    //! @brief Extrae la raíz del heap y reordena con `heapify_down`.
    //! @throw std::out_of_range Si el heap está vacío.
    void extract() {
        if (m_heap.empty())
            throw std::out_of_range("Heap is empty");
        if (m_heap.size() == 1) {
            m_heap.pop_back();
            return;
        }
        m_heap[0] = m_heap.back();
        m_heap.pop_back();
        heapify_down(0);
    }

    //! @brief Devuelve (por copia) el valor de la raíz sin extraerla.
    //! @throw std::out_of_range Si el heap está vacío.
    T peek_min() const {
        if (m_heap.empty())
            throw std::out_of_range("Heap is empty");
        return m_heap[0];
    }

    //! @brief `true` si el heap está vacío.
    bool empty() const { return m_heap.empty(); }

    //! @brief Número de elementos en el heap.
    size_t size() const { return m_heap.size(); }

private:
    //! @brief Sube un nodo mientras sea menor que su padre según `Comp`.
    //! @param index Índice inicial.
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if ( m_comp(m_heap[index], m_heap[parent]) ) {
                std::swap(m_heap[index], m_heap[parent]);
                index = parent;
            } else
                break;
        }
    }

    //! @brief Hunde un nodo mientras sea mayor que el mejor de sus hijos.
    //! @param index Índice inicial.
    void heapify_down(size_t index) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;

        if (left  < m_heap.size() && m_comp(m_heap[left ], m_heap[smallest]))
            smallest = left;
        if (right < m_heap.size() && m_comp(m_heap[right], m_heap[smallest]))
            smallest = right;

        if (smallest != index) {
            std::swap(m_heap[index], m_heap[smallest]);
            heapify_down(smallest);
        }
    }
};

#endif // __HEAP_H__
