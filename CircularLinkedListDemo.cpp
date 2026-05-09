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
    size_t n = list.size() * 2;

    for(size_t i = 0; i < n; ++i){
        cout << *it;

        if(i != n - 1)
            cout << ",";

        ++it;
    }

    cout << endl;
}