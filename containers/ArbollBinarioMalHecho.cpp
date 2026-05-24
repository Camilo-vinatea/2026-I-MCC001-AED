#include <iostream>

// Estructura para el Nodo
struct Nodo {
    int valor;
    Nodo* izquierdo;
    Nodo* derecho;

    // Constructor del nodo
    Nodo(int val) : valor(val), izquierdo(nullptr), derecho(nullptr) {}
};

class ArbolBinario {
private:
    Nodo* raiz;

    // Método privado recursivo para insertar
    Nodo* insertarRecursivo(Nodo* nodoActual, int valor) {
        // Si el subárbol está vacío, creamos el nodo aquí
        if (nodoActual == nullptr) {
            return new Nodo(valor);
        }

        // Si no, bajamos por el árbol
        if (valor < nodoActual->valor) {
            nodoActual->izquierdo = insertarRecursivo(nodoActual->izquierdo, valor);
        } else {
            nodoActual->derecho = insertarRecursivo(nodoActual->derecho, valor);
        }

        return nodoActual;
    }

    // Método privado para el recorrido en orden
    void mostrarEnOrdenRecursivo(Nodo* nodoActual) {
        if (nodoActual != nullptr) {
            mostrarEnOrdenRecursivo(nodoActual->izquierdo); // Izquierda
            std::cout << nodoActual->valor << " ";         // Raíz
            mostrarEnOrdenRecursivo(nodoActual->derecho);   // Derecha
        }
    }

    // Método privado para liberar la memoria (Destructor)
    void liberarMemoria(Nodo* nodoActual) {
        if (nodoActual != nullptr) {
            liberarMemoria(nodoActual->izquierdo);
            liberarMemoria(nodoActual->derecho);
            delete nodoActual;
        }
    }

public:
    // Constructor del árbol
    ArbolBinario() : raiz(nullptr) {}

    // Destructor para evitar fugas de memoria (Memory Leaks)
    ~ArbolBinario() {
        liberarMemoria(raiz);
    }

    // Métodos públicos que interactúan con el usuario
    void insertar(int valor) {
        raiz = insertarRecursivo(raiz, valor);
    }

    void mostrarEnOrden() {
        mostrarEnOrdenRecursivo(raiz);
        std::cout << std::endl;
    }
};

int main() {
    ArbolBinario arbol;

    // Insertamos los mismos datos que en el ejemplo anterior
    arbol.insertar(50);
    arbol.insertar(30);
    arbol.insertar(70);
    arbol.insertar(20);
    arbol.insertar(40);
    arbol.insertar(60);
    arbol.insertar(80);

    std::cout << "Recorrido en orden (In-order) en C++:" << std::endl;
    arbol.mostrarEnOrden();

    return 0;
}