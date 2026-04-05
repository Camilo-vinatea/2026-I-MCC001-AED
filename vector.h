

class Vector{
    private:
        int *m_data;
        int  m_size;
        int m_capacity;
    public:
        Vector();
        ~Vector();
        void push_back(int value);
        int get(int index);
        int size();
};

void DemoVector();