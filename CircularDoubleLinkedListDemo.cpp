#include "containers/circulardoublelinkedlist.h"

void CircularDoubleLinkedListDemo(){

    CircularDoubleLinkedList<
        AscendingDoubleLinkedListTrait<TI>
    > list;

    list.insert(1, 1);
    list.insert(2, 2);
    list.insert(3, 3);
    list.insert(4, 4);

    cout << "Forward circular (2 vueltas): " << endl;

    auto it = list.begin();

    for(size_t i = 0; i < list.size() * 2; ++i){
        cout << *it << ",";
        ++it;
    }

    cout << endl;

    cout << "Backward circular (2 vueltas): " << endl;

    auto rit = list.rbegin();

    for(size_t i = 0; i < list.size() * 2; ++i){
        cout << *rit << ",";
        ++rit;
    }

    cout << endl;
}
