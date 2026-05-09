#include "containers/circulardoublelinkedlist.h"

void CircularDoubleLinkedListDemo(){

    CircularDoubleLinkedList<
        AscendingDoubleLinkedListTrait<TI>
    > list;

    list.insert(1, 1);
    list.insert(2, 2);
    list.insert(3, 3);
    list.insert(4, 4);

    size_t n = list.size() * 2;
    
    cout << "Forward circular (2 vueltas): " << endl;

    auto it = list.begin();

    for(size_t i = 0; i < n; ++i){
        cout << *it;
        if(i != n - 1)
            cout << ",";
        ++it;
    }

    cout << endl;

    cout << "Backward circular (2 vueltas): " << endl;

    auto rit = list.rbegin();


    for(size_t i = 0; i < n; ++i){
        cout << *rit;
        if(i != n - 1)
            cout << ",";
        ++rit;
    }
    cout << endl;
}
