#include <iostream>
#include <fstream>
#include "containers/binarytree.h"

using namespace std;

void DemoBinaryTree() {
    cout << "\n=== DemoBinaryTree ===" << endl;

    // --- Build BST: AscendingBinaryTreeListTrait<TI> ---
    BinaryTree<AscendingBinaryTreeListTrait<TI>> bt;
    bt.insert(50, 0);
    bt.insert(30, 1);
    bt.insert(70, 2);
    bt.insert(20, 3);
    bt.insert(40, 4);
    bt.insert(60, 5);
    bt.insert(80, 6);

    cout << "Size: " << bt.size() << endl;
    cout << "toString (inorder): " << bt.toString() << endl;

    auto printNode = [](auto& node) {
        cout << node.getData() << " ";
    };

    cout << "\nInorder Forward  (LNR): ";
    bt.ForEach(printNode);
    cout << endl;

    cout << "Inorder Backward (RNL): ";
    bt.ReverseForEach(printNode);
    cout << endl;

    cout << "Preorder Forward  (NLR): ";
    bt.ForEachPreorder(printNode);
    cout << endl;

    cout << "Preorder Backward (NRL): ";
    bt.ForEachReversePreorder(printNode);
    cout << endl;

    cout << "Postorder Forward  (LRN): ";
    bt.ForEachPostorder(printNode);
    cout << endl;

    cout << "Postorder Backward (RLN): ";
    bt.ForEachReversePostorder(printNode);
    cout << endl;

    // --- FirstThat ---
    auto found = bt.FirstThat([](auto& node) {
        return node.getData() > 35;
    });
    if (found != bt.end())
        cout << "\nFirstThat(data > 35): " << (*found).getData() << endl;

    // --- Copy Constructor ---
    BinaryTree<AscendingBinaryTreeListTrait<TI>> bt2(bt);
    cout << "\nCopy (inorder): ";
    bt2.ForEach(printNode);
    cout << endl;

    // --- Move Constructor ---
    BinaryTree<AscendingBinaryTreeListTrait<TI>> bt3(move(bt2));
    cout << "Move (inorder, source empty=" << bt2.empty() << "): ";
    bt3.ForEach(printNode);
    cout << endl;

    // --- Disk write ---
    {
        ofstream file("binarytree.txt");
        file << bt;
        cout << "\nWritten to binarytree.txt" << endl;
    }

    // --- Disk read ---
    {
        BinaryTree<AscendingBinaryTreeListTrait<TI>> bt4;
        ifstream file("binarytree.txt");
        file >> bt4;
        cout << "Read from disk (inorder): ";
        bt4.ForEach(printNode);
        cout << endl;
    }

    // --- Descending tree ---
    BinaryTree<DescendingBinaryTreeListTrait<TI>> btDesc;
    btDesc.insert(50, 0);
    btDesc.insert(30, 1);
    btDesc.insert(70, 2);
    btDesc.insert(20, 3);
    btDesc.insert(40, 4);
    cout << "\nDescending BST inorder: ";
    btDesc.ForEach(printNode);
    cout << endl;

    cout << "=== DemoBinaryTree done ===" << endl;
}
