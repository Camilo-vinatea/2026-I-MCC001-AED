#include "containers/doublelinkedlist.h"
#include <fstream>

template <typename Node>
void Print(Node &node, ostream& os){
    os << node;
}

template <typename Node>
void AddX(Node &node, typename Node::value_type value){
    node.getDataRef() += value;
}

template <typename Node>
void AddY(Node &node,
          typename Node::value_type value1,
          typename Node::value_type value2){
    node.getDataRef() += value1 + value2;
}

template <typename Node, typename T>
bool IsGreaterThan(Node &node, T x){
    return node.getDataRef() > x;
}

void DoubleLinkedListDemo(){

    ////////////////////////////////////////////////////////////
    // Insert (lista ordenada descendente)
    ////////////////////////////////////////////////////////////
    DoubleLinkedList<DescendingDoubleLinkedListTrait<TI>> list1;

    list1.insert(6, 15);
    list1.insert(2, 25);
    list1.insert(9, 35);
    list1.insert(1, 45);
    list1.insert(7, 55);

    cout << "Double List descendente: "
         << list1 << endl;


    ////////////////////////////////////////////////////////////
    // Insert (lista ordenada ascendente)
    ////////////////////////////////////////////////////////////
    DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> list2;

    using LI =
        DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>>::Node;

    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);

    cout << "Double List ascendente: "
         << list2 << endl;


    ////////////////////////////////////////////////////////////
    // ForEach
    ////////////////////////////////////////////////////////////
    list2.ForEach(AddX<LI>, 3);

    cout << "Prueba ForEach + 3: " << endl;
    list2.ForEach(Print<LI>, cout);
    cout << endl;

    list2.ForEach(AddY<LI>, 10, 11);

    cout << "Prueba ForEach + 10 + 11: " << endl;
    list2.ForEach(Print<LI>, cout);
    cout << endl;


    ////////////////////////////////////////////////////////////
    // FirstThat
    ////////////////////////////////////////////////////////////
    cout << "Prueba FirstThat: " << endl;

    auto it =
        list2.FirstThat(IsGreaterThan<LI, TI>, 20);

    if(it != list2.end())
        cout << "Primer mayor a 20: "
             << *it << endl;


    ////////////////////////////////////////////////////////////
    // Forward Iterator
    ////////////////////////////////////////////////////////////
    cout << "Prueba Forward Iterator: " << endl;

    for(auto it = list2.begin();
        it != list2.end();
        ++it){

        cout << *it << ",";
    }

    cout << endl;


    ////////////////////////////////////////////////////////////
    // Backward Iterator
    ////////////////////////////////////////////////////////////
    cout << "Prueba Backward Iterator: " << endl;

    for(auto it = list2.rbegin();
        it != list2.rend();
        ++it){

        cout << *it << ",";
    }

    cout << endl;


    ////////////////////////////////////////////////////////////
    // Push Front
    ////////////////////////////////////////////////////////////
    cout << "Prueba PushFront: " << endl;

    list1.push_front(20, 100);

    cout << "Lista 1: "
         << list1 << endl;


    ////////////////////////////////////////////////////////////
    // Pop Front
    ////////////////////////////////////////////////////////////
    cout << "Prueba PopFront: " << endl;

    list1.pop_front();

    cout << "Lista 1: "
         << list1 << endl;


    ////////////////////////////////////////////////////////////
    // Push Back
    ////////////////////////////////////////////////////////////
    DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> list3;

    cout << "Prueba PushBack: " << endl;

    list3.push_back(1, 10);
    list3.push_back(2, 20);
    list3.push_back(3, 30);

    cout << "Lista 3: "
         << list3 << endl;


    ////////////////////////////////////////////////////////////
    // Pop Back
    ////////////////////////////////////////////////////////////
    cout << "Prueba PopBack: " << endl;

    list3.pop_back();

    cout << "Lista 3: "
         << list3 << endl;


    ////////////////////////////////////////////////////////////
    // Copy Constructor
    ////////////////////////////////////////////////////////////
    list3.push_back(4, 40);

    cout << "Prueba Copy Constructor: "
         << endl;

    DoubleLinkedList<
        AscendingDoubleLinkedListTrait<TI>
    > list4(list3);

    cout << "Lista 4: "
         << list4 << endl;


    ////////////////////////////////////////////////////////////
    // Operador >>
    ////////////////////////////////////////////////////////////
    cout << "Prueba operador >>: "
         << endl;

    ofstream ofs;
    ofs.open("doublelist.txt");

    ofs << list1 << endl;

    ofs.close();

    ifstream file("doublelist.txt");

    DoubleLinkedList<AscendingDoubleLinkedListTrait<TI>> list5;

    file >> list5;

    cout << "Lista 5 cargada desde archivo: "
         << list5 << endl;


    ////////////////////////////////////////////////////////////
    // Operador []
    ////////////////////////////////////////////////////////////
    cout << "Prueba operador []: "
         << endl;

    cout << "Lista4[2]: "
         << list4[2] << endl;
}

void ListsDemo(){
    DoubleLinkedListDemo();
}