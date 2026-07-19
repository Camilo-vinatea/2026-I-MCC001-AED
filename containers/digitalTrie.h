//! @file digitalTrie.h
//! @brief Trie digital (árbol de prefijos) ordenado con iterador e/ concurrencia.
//! @details Implementa `DigitalTrie<T>`: cada clave es un `string` y cada nodo
//!          guarda sus hijos en un `std::map<char, Node*>` (tipo no nativo),
//!          lo que mantiene el recorrido en orden lexicográfico. Reutiliza la
//!          arquitectura del proyecto: `general_iterator` (CRTP), `types.h`,
//!          `foreach.h` (`ForEach`/`FirstThat`) y `std::mutex`/`scoped_lock`
//!          para la protección concurrente.
//! @author Equipo MCC

#ifndef __DIGITAL_TRIE_H__
#define __DIGITAL_TRIE_H__

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <mutex>
#include <utility>
#include <algorithm>
#include "general_iterator.h"
#include "../types.h"
#include "../foreach.h"

using namespace std;

//! @class DigitalTrieForwardIterator
//! @brief Iterador forward que recorre las claves terminales en preorden
//!        (orden lexicográfico) siguiendo la topología del trie.
template <typename Container>
class DigitalTrieForwardIterator
    : public general_iterator<Container, DigitalTrieForwardIterator<Container>>
{
    using MySelf = DigitalTrieForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    //! @brief Avanza al siguiente nodo terminal en preorden.
    MySelf& operator++() {
        do { this->m_pNode = this->m_pNode->nextPreorder(); }
        while (this->m_pNode && !this->m_pNode->isEnd());
        return *this;
    }
};

//! @class DigitalTrie
//! @brief Trie digital ordenado sobre claves `string` con valor asociado `T`.
//! @tparam T Tipo de dato almacenado en cada clave terminal (por defecto `Ref`).
template <typename T = Ref>
class DigitalTrie {
public:
    //! @struct Node
    //! @brief Nodo del trie: carácter, dato, `Ref`, marca terminal, padre e hijos.
    struct Node {
        using value_type = T;
        char             m_char;                //!< Carácter que etiqueta la arista al padre.
        T                m_data{};              //!< Valor almacenado (si es terminal).
        Ref              m_ref{};               //!< Identificador asociado.
        bool             m_isEnd = false;       //!< `true` si aquí termina una clave.
        Node*            m_pParent;             //!< Nodo padre (nullptr en la raíz).
        map<char, Node*> m_children;            //!< Hijos ordenados por carácter.

        //! @brief Constructor: carácter y padre.
        Node(char c = '\0', Node* parent = nullptr)
            : m_char(c), m_pParent(parent) {}

        //! @brief Destructor recursivo: libera todo el subárbol de hijos.
        ~Node() { for (auto& kv : m_children) delete kv.second; }

        T      getData()    const { return m_data; }
        T&     getDataRef()       { return m_data; }
        void   setData(T d)       { m_data = d; }
        Ref    getRef()     const { return m_ref; }
        Ref&   getRefRef()        { return m_ref; }
        bool   isEnd()      const { return m_isEnd; }

        //! @brief Reconstruye la clave completa subiendo hasta la raíz.
        string getKey() const {
            string s;
            for (const Node* n = this; n->m_pParent; n = n->m_pParent)
                s.push_back(n->m_char);
            reverse(s.begin(), s.end());
            return s;
        }

        //! @brief Siguiente nodo en preorden (hijo menor, si no, hermano/tío).
        //! @return Puntero al siguiente nodo, o `nullptr` al terminar.
        Node* nextPreorder() {
            if (!m_children.empty()) return m_children.begin()->second;
            for (Node* n = this; n->m_pParent; n = n->m_pParent) {
                auto& sib = n->m_pParent->m_children;
                auto it = sib.upper_bound(n->m_char);
                if (it != sib.end()) return it->second;
            }
            return nullptr;
        }

        //! @brief Imprime el nodo como `("clave", data, ref)`.
        friend ostream& operator<<(ostream& os, const Node& n) {
            return os << "(\"" << n.getKey() << "\", " << n.m_data << ", " << n.m_ref << ")";
        }
    };

    using value_type       = T;
    using MySelf           = DigitalTrie<T>;
    using forward_iterator = DigitalTrieForwardIterator<MySelf>;

protected:
    Node*         m_pRoot = new Node();  //!< Raíz centinela (clave vacía).
    size_t        m_size  = 0;           //!< Número de claves almacenadas.
    mutable mutex m_mtx;                 //!< Mutex para concurrencia.

public:
    //! @brief Constructor por defecto.
    DigitalTrie() = default;

    //! @brief Constructor de copia: reinserta todas las claves del otro trie.
    DigitalTrie(const DigitalTrie& other) {
        for (auto it = other.cbegin(); it != other.cend(); ++it)
            insert(it->getKey(), it->getData(), it->getRef());
    }

    //! @brief Constructor de movimiento: toma el control del otro trie.
    DigitalTrie(DigitalTrie&& other) noexcept {
        scoped_lock<mutex> lock(other.m_mtx);
        delete m_pRoot;
        m_pRoot = exchange(other.m_pRoot, new Node());
        m_size  = exchange(other.m_size, size_t{0});
    }

    //! @brief Asignación por copia.
    DigitalTrie& operator=(const DigitalTrie& other) {
        if (this != &other) {
            clear();
            for (auto it = other.cbegin(); it != other.cend(); ++it)
                insert(it->getKey(), it->getData(), it->getRef());
        }
        return *this;
    }

    //! @brief Asignación por movimiento.
    DigitalTrie& operator=(DigitalTrie&& other) noexcept {
        if (this != &other) {
            scoped_lock<mutex> lock(m_mtx, other.m_mtx);
            delete m_pRoot;
            m_pRoot = exchange(other.m_pRoot, new Node());
            m_size  = exchange(other.m_size, size_t{0});
        }
        return *this;
    }

    //! @brief Destructor: libera todo el árbol (recursivo vía `~Node`).
    virtual ~DigitalTrie() { delete m_pRoot; }

    //! @brief Inserta o actualiza la clave `key` con `(value, ref)`.
    void insert(const string& key, T value, Ref ref = Ref{}) {
        scoped_lock<mutex> lock(m_mtx);
        Node* cur = m_pRoot;
        for (char c : key) {
            auto it = cur->m_children.find(c);
            if (it == cur->m_children.end())
                cur = cur->m_children[c] = new Node(c, cur);
            else
                cur = it->second;
        }
        if (!cur->m_isEnd) { cur->m_isEnd = true; ++m_size; }
        cur->m_data = value;
        cur->m_ref  = ref;
    }

    //! @brief Acceso tipo `std::map`: devuelve referencia al valor de `key`,
    //!        creando la clave (con `T{}`) si no existía. Solo toca `m_data`.
    T& operator[](const string& key) {
        scoped_lock<mutex> lock(m_mtx);
        Node* cur = m_pRoot;
        for (char c : key) {
            auto it = cur->m_children.find(c);
            if (it == cur->m_children.end())
                cur = cur->m_children[c] = new Node(c, cur);
            else
                cur = it->second;
        }
        if (!cur->m_isEnd) { cur->m_isEnd = true; ++m_size; }
        return cur->m_data;
    }

    //! @brief Localiza el nodo terminal de `key`, o `nullptr` si no existe.
    Node* find(const string& key) const {
        Node* cur = m_pRoot;
        for (char c : key) {
            auto it = cur->m_children.find(c);
            if (it == cur->m_children.end()) return nullptr;
            cur = it->second;
        }
        return cur->m_isEnd ? cur : nullptr;
    }

    //! @brief `true` si la clave exacta está almacenada.
    bool contains(const string& key) const { return find(key) != nullptr; }

    //! @brief `true` si algún nodo cuelga del prefijo `prefix`.
    bool startsWith(const string& prefix) const {
        Node* cur = m_pRoot;
        for (char c : prefix) {
            auto it = cur->m_children.find(c);
            if (it == cur->m_children.end()) return false;
            cur = it->second;
        }
        return true;
    }

    //! @brief Elimina la clave `key` y poda los nodos que quedan sin uso.
    //! @return `true` si la clave existía y fue eliminada.
    bool erase(const string& key) {
        scoped_lock<mutex> lock(m_mtx);
        Node* cur = m_pRoot;
        for (char c : key) {
            auto it = cur->m_children.find(c);
            if (it == cur->m_children.end()) return false;
            cur = it->second;
        }
        if (!cur->m_isEnd) return false;
        cur->m_isEnd = false;
        --m_size;
        // Poda hacia arriba los nodos hoja no terminales.
        while (cur->m_pParent && cur->m_children.empty() && !cur->m_isEnd) {
            Node* p = cur->m_pParent;
            p->m_children.erase(cur->m_char);
            cur->m_children.clear();  // evita doble free en ~Node
            delete cur;
            cur = p;
        }
        return true;
    }

    //! @brief Número de claves almacenadas.
    size_t size()  const { return m_size; }
    //! @brief `true` si el trie está vacío.
    bool   empty() const { return m_size == 0; }

    //! @brief Vacía el trie dejándolo con solo la raíz.
    void clear() {
        scoped_lock<mutex> lock(m_mtx);
        delete m_pRoot;
        m_pRoot = new Node();
        m_size  = 0;
    }

    //! @brief Representación textual `["a",..],["ab",..],...`.
    string toString() const {
        stringstream ss;
        ss << "[";
        bool first = true;
        for (Node* p = firstTerminal(); p; p = advance(p)) {
            if (!first) ss << ",";
            ss << *p;
            first = false;
        }
        ss << "]";
        return ss.str();
    }

    //! @brief Dibuja el trie como árbol ASCII indentado; los nodos terminales
    //!        se marcan con `*`. Evidencia prefijos compartidos y ramificación.
    void printTree(ostream& os = cout) const {
        os << "(raiz)\n";
        printSubtree(m_pRoot, "", os);
    }

    //! @brief Iterador a la primera clave (menor en orden lexicográfico).
    forward_iterator begin() { return {this, firstTerminal()}; }
    //! @brief Iterador "uno más allá" de la última clave.
    forward_iterator end()   { return {this, nullptr}; }

    //! @brief Iteradores constantes (para copia y recorrido de solo lectura).
    forward_iterator cbegin() const { return {const_cast<MySelf*>(this), firstTerminal()}; }
    forward_iterator cend()   const { return {const_cast<MySelf*>(this), nullptr}; }

    //! @brief Recorre las claves aplicando `func(nodo, args...)` bajo mutex.
    template <typename Func, typename... Args>
    void ForEach(Func func, Args&&... args) {
        unique_lock<mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, forward<Args>(args)...);
    }

    //! @brief Primer nodo cuyo predicado devuelve `true` (o `end()`).
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args&&... args) {
        return ::FirstThat(begin(), end(), func, forward<Args>(args)...);
    }

private:
    //! @brief Recorrido recursivo para `printTree`: dibuja hijos ordenados.
    void printSubtree(Node* node, const string& prefix, ostream& os) const {
        size_t i = 0, n = node->m_children.size();
        for (auto& [c, child] : node->m_children) {
            bool last = (++i == n);
            os << prefix << (last ? "\\-- " : "|-- ")
               << c << (child->m_isEnd ? "*" : "") << "\n";
            printSubtree(child, prefix + (last ? "    " : "|   "), os);
        }
    }

    //! @brief Primer nodo terminal en preorden desde la raíz.
    Node* firstTerminal() const {
        Node* p = m_pRoot;
        while (p && !p->m_isEnd) p = p->nextPreorder();
        return p;
    }
    //! @brief Siguiente nodo terminal a partir de `p`.
    Node* advance(Node* p) const {
        do { p = p->nextPreorder(); } while (p && !p->m_isEnd);
        return p;
    }
};

//! @brief Operador `<<` para imprimir el trie completo.
template <typename T>
ostream& operator<<(ostream& os, const DigitalTrie<T>& trie) {
    return os << trie.toString();
}

void DigitalTrieDemo();

#endif // __DIGITAL_TRIE_H__
