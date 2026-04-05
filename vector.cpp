#include <iostream>
#include "vector.h"

using namespace std;

Vector::Vector() {
    m_data = nullptr;
    m_size = 0;
    m_capacity = 0;
}

Vector::~Vector() {
    delete[] m_data;
}

void Vector::push_back(int value) {
    if (m_size == m_capacity) {
        m_capacity = m_capacity == 0 ? 1 : m_capacity * 2;
        int *newData = new int[m_capacity];
        for (int i = 0; i < m_size; i++)
            newData[i] = m_data[i];
        delete[] m_data;
        m_data = newData;
    }
    m_data[m_size] = value;
    m_size++;
}

int Vector::get(int index) {
    return m_data[index];
}

int Vector::size() {
    return m_size;
}

void DemoVector(){
    Vector v;
    v.push_back(7);
    v.push_back(5);
    v.push_back(25);
    v.push_back(10);
    v.push_back(15);

    for(auto i = 0; i < v.size(); ++i){
        cout << v.get(i) << " ";
    }
    cout << endl;
    cout << "Size: " << v.size() << endl;
}