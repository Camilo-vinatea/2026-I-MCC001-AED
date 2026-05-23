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

    bool first = true;
    for(int cycle = 0; cycle < 2; ++cycle){
        for(auto it = list.begin(); it != list.end(); ++it){
            if(!first) cout << ",";
            cout << *it;
            first = false;
        }
    }

    cout << endl;

    cout << "Backward circular (2 vueltas): " << endl;

    first = true;
    for(int cycle = 0; cycle < 2; ++cycle){
        for(auto rit = list.rbegin(); rit != list.rend(); ++rit){
            if(!first) cout << ",";
            cout << *rit;
            first = false;
        }
    }

    cout << endl;
}
