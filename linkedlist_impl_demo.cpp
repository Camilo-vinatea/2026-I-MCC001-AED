#include <iostream>
#include <sstream>
#include "linkedlist.h"

using namespace std;

// ── Helpers con la misma convención que vector.cpp ───────────────────────────

template <typename Node>
void PrintNode(Node& node, ostream& os){
    os << node << " ";
}

template <typename Node>
bool MayorQue(Node& node, int umbral){
    return node.getData() > umbral;
}

template <typename Node>
bool IgualA(Node& node, int valor){
    return node.getData() == valor;
}

// ── Demo ─────────────────────────────────────────────────────────────────────

void LinkedListImplDemo(){
    using Trait = AscendingLinkedListTrait<TI>;
    using List  = LinkedList<Trait>;

    // ── 1. push_front / push_back ─────────────────────────────────────────────
    cout << "\n[1] push_front / push_back" << endl;
    List list1;
    list1.push_back(30, 3);   //  [30]
    list1.push_back(50, 5);   //  [30, 50]
    list1.push_front(10, 1);  //  [10, 30, 50]
    list1.push_front(5,  0);  //  [5, 10, 30, 50]
    cout << "  lista : " << list1 << endl;
    cout << "  size  : " << list1.size() << endl;

    // ── 2. operator[] ─────────────────────────────────────────────────────────
    cout << "\n[2] operator[]" << endl;
    cout << "  list1[0] : " << list1[0] << endl;
    cout << "  list1[2] : " << list1[2] << endl;
    list1[1].setData(99);
    cout << "  list1[1].setData(99) -> " << list1 << endl;
    list1[1].setData(10);     // restaurar para los siguientes demos
    try {
        list1[99];
    } catch(const out_of_range& e){
        cout << "  excepcion esperada -> " << e.what() << endl;
    }

    // ── 3. pop_back ───────────────────────────────────────────────────────────
    cout << "\n[3] pop_back" << endl;
    auto [data, ref] = list1.pop_back();
    cout << "  extraido  : (" << data << ", " << ref << ")" << endl;
    cout << "  lista     : " << list1 << endl;
    cout << "  size      : " << list1.size() << endl;

    // ── 4. FirstThat ──────────────────────────────────────────────────────────
    cout << "\n[4] FirstThat" << endl;
    auto it = list1.FirstThat(MayorQue<LLNode<TI>>, 8);
    if(it != list1.end())
        cout << "  primero > 8  : " << *it << endl;
    else
        cout << "  primero > 8  : no encontrado" << endl;

    auto it2 = list1.FirstThat(IgualA<LLNode<TI>>, 999);
    cout << "  buscar 999   : " << (it2 == list1.end() ? "no encontrado" : "encontrado") << endl;

    // ── 5. ForEach con helper y con lambda ────────────────────────────────────
    cout << "\n[5] ForEach" << endl;
    cout << "  helper  : ";
    list1.ForEach(PrintNode<LLNode<TI>>, cout);
    cout << endl;

    // ── 6. Copy constructor ───────────────────────────────────────────────────
    cout << "\n[6] Copy constructor" << endl;
    List list2(list1);
    list2.push_back(99, 9);
    cout << "  original : " << list1 << endl;
    cout << "  copia    : " << list2 << "  (modificacion independiente)" << endl;

    // ── 7. Move constructor ───────────────────────────────────────────────────
    cout << "\n[7] Move constructor" << endl;
    List list3(std::move(list2));
    cout << "  movida         : " << list3 << endl;
    cout << "  fuente (vacia) : size=" << list2.size() << endl;

    // ── 8. operator>> (lectura desde stringstream) ────────────────────────────
    cout << "\n[8] operator>>" << endl;
    List list4;
    istringstream ss("100 10  200 20  300 30");
    while(ss >> list4) {}
    cout << "  leida desde stream : " << list4 << endl;

    // pop_back hasta vaciar para verificar todos los nodos
    cout << "  pop_back x3 : ";
    while(list4.size() > 0){
        auto [d, r] = list4.pop_back();
        cout << "(" << d << "," << r << ") ";
    }
    cout << endl;

    // ── 9. Destructor seguro ──────────────────────────────────────────────────
    cout << "\n[9] Destructor seguro" << endl;
    {
        List temp;
        temp.push_back(1, 10);
        temp.push_back(2, 20);
        temp.push_back(3, 30);
        cout << "  dentro del scope  : " << temp << endl;
    } // ~LinkedList() libera los 3 nodos aqui
    cout << "  fuera del scope   : destructor libero la cadena sin leak" << endl;

}
