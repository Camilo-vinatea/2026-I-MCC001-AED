#include "linkedlist.h"

template <typename Node>
void Print(Node &value, ostream& os){
    os << value << ",";
}

template <typename Node, typename T>
bool IsGreaterThan(Node &node, T x){
    return node.getDataRef() > x;
}

void LinkedListDemo(){
    // 
    LinkedList<DescendingLinkedListTrait<TI>> list1;
    list1.insert(6, 15);
    list1.insert(2, 25);
    list1.insert(9, 35);
    list1.insert(1, 45);
    list1.insert(7, 55);
    cout << "Lista descendente: " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2;
    using LI = LinkedList<AscendingLinkedListTrait<TI>>::Node;
    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);
    cout << "Lista ascendente : " << list2 << endl;

    cout << "Prueba ForEach: " << endl;
    list2.ForEach(Print<LI>, cout);
    
    cout << "Prueba First That: " << endl;
    auto it = list2.FirstThat(IsGreaterThan<LI, TI>, 7);
    if (it != list2.end())
        cout << "Primer mayor a 7   : " << *it << endl;
    cout << "Fin recorrido con iteradores" << endl;

    cout << "Prueba PushFront: " << endl;
    list1.push_front(5, 5);
    cout << "Lista ascendente : " << list1 << endl;

    cout << "Prueba PopFront: " << endl;
    list1.pop_front();
    cout << "Lista ascendente : " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list3;
    cout << "Prueba PushBack: " << endl;
    list3.push_back(1, 10);
    list3.push_back(2, 20);
    list3.push_back(3, 30);
    cout << "Lista ascendente : " << list3 << endl;

    cout << "Prueba PopBack: " << endl;
    list3.pop_back();
    cout << "Lista ascendente : " << list3 << endl;

}

void ListsDemo(){
    LinkedListDemo();
}
