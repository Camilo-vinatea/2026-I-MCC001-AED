#include "containers/circularlinkedlist.h"

void CircularLinkedListDemo(){

    CircularLinkedList<
        AscendingLinkedListTrait<TI>
    > list;

    list.insert(10, 100);
    list.insert(20, 200);
    list.insert(30, 300);
    list.insert(40, 400);

    cout << "Recorrido circular (2 vueltas): "
         << endl;

    auto it = list.begin();

    for(size_t i = 0; i < list.size()*2; ++i){
        cout << *it << ",";
        ++it;
    }

    cout << endl;
}