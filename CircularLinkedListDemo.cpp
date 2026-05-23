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

    bool first = true;
    for(int cycle = 0; cycle < 2; ++cycle){
        for(auto it = list.begin(); it != list.end(); ++it){
            if(!first) cout << ",";
            cout << *it;
            first = false;
        }
    }

    cout << endl;
}
