#include "containers/heap.h"
#include <fstream>

template <typename Node>
void PrintNode(Node& node, ostream& os) {
    os << node << " ";
}

static void section(const char* title) {
    cout << "\n=== " << title << " ===" << endl;
}

void HeapDemo() {

    section("Max Heap (Descending) -- extrae mayor primero");
    Heap<DescendingHeapTrait<TI>> maxHeap;
    maxHeap.insert(10);
    maxHeap.insert(30);
    maxHeap.insert(20);
    maxHeap.insert(5);
    maxHeap.insert(40);
    maxHeap.insert(15);
    cout << "  Array interno: " << maxHeap << endl;
    cout << "  Peek (maximo): " << maxHeap.peek() << endl;
    cout << "  Extraccion:    ";
    while (!maxHeap.empty())
        cout << maxHeap.extract() << " ";
    cout << endl;


    section("Min Heap (Ascending) -- extrae menor primero");
    Heap<AscendingHeapTrait<TI>> minHeap;
    minHeap.insert(10);
    minHeap.insert(30);
    minHeap.insert(20);
    minHeap.insert(5);
    minHeap.insert(40);
    minHeap.insert(15);
    cout << "  Array interno: " << minHeap << endl;
    cout << "  Peek (minimo): " << minHeap.peek() << endl;
    cout << "  Extraccion:    ";
    while (!minHeap.empty())
        cout << minHeap.extract() << " ";
    cout << endl;


    section("operator<< / operator>> (stream)");
    Heap<AscendingHeapTrait<TI>> heap1;
    heap1.insert(7);
    heap1.insert(2);
    heap1.insert(9);
    ofstream ofs("heap.txt");
    ofs << heap1 << endl;
    ofs.close();
    Heap<AscendingHeapTrait<TI>> heap2;
    ifstream ifs("heap.txt");
    ifs >> heap2;
    ifs.close();
    cout << "  Guardado: " << heap1 << endl;
    cout << "  Cargado:  " << heap2 << endl;


    section("Copy Constructor");
    Heap<DescendingHeapTrait<TI>> heap3;
    heap3.insert(50);
    heap3.insert(30);
    heap3.insert(10);
    Heap<DescendingHeapTrait<TI>> heap4(heap3);
    cout << "  Original: " << heap3 << endl;
    cout << "  Copia:    " << heap4 << endl;


    section("Move Constructor");
    Heap<DescendingHeapTrait<TI>> heap5(move(heap3));
    cout << "  Movido:             " << heap5 << endl;
    cout << "  Original tras move: size=" << heap3.size() << endl;


    section("Mejora #1 -- build O(n) via Floyd");
    TI values[] = { 15, 3, 22, 8, 1, 40 };
    Heap<AscendingHeapTrait<TI>> heap6;
    heap6.build(values, 6);
    cout << "  Input:       { 15, 3, 22, 8, 1, 40 }" << endl;
    cout << "  Array heap:  " << heap6 << endl;
    cout << "  Peek minimo: " << heap6.peek() << endl;


    section("Mejora #2 -- replace (O(log n))");
    Heap<AscendingHeapTrait<TI>> heap7;
    heap7.insert(5);
    heap7.insert(10);
    heap7.insert(15);
    cout << "  Antes:       " << heap7 << endl;
    auto old = heap7.replace(1);
    cout << "  replace(1) saca: " << old << endl;
    cout << "  Despues:     " << heap7 << endl;


    section("ForEach (orden interno del arreglo)");
    using N = Heap<AscendingHeapTrait<TI>>::Node;
    cout << "  heap7: ";
    heap7.ForEach(PrintNode<N>, cout);
    cout << endl;
}
