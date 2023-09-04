#pragma once
#include <iostream>
using namespace std;
template <typename T>
class CircularList
{
private:
    // smartpointer is considering as cheating so no smartpointer here
    T *storage = nullptr;
    T *tempStorage = nullptr;
    int s;
    int cap;
    int head;
    int tail;
    void makeNewOnHeap()
    {
        tempStorage = storage;
        storage = new T[s * 2];
        for (int i = 0; i < cap; ++i)
        {
            *(storage + i) = *(tempStorage + (head + i) % cap);
        }
        head = 0;
        delete[] tempStorage;
        cap = s * 2;
    }
   
public:
    CircularList() // default constructor
    {
        s = cap = 0;
    }
    CircularList(int ca)
    {
        cap = ca;
        s = ca;
        storage = new T[ca];
        head = 0;
        tail = 0;
    }
    CircularList(int ca, T value)
    {
        cap = ca;
        s = ca;
        storage = new T[ca];
        head = 0;
        tail = ca;
        for (int i = 0; i < ca; ++i)
            *(storage + i) = value;
    }
    ~CircularList() // destructor that clear all active pointers
    {
        if (storage != nullptr)
        {
            delete[] storage;
        }
    }
    CircularList(const CircularList &source) // copy constructor: make new memory location on heap
    {
        s = source.size();
        cap = source.cap;
        storage = new T[cap];
        head = source.head;
        tail = source.tail;
        for (int i = 0; i < s; ++i)
        {
            *(storage + (head + i) % cap) = *(source.storage + +(head + i) % cap);
        }
    }
    CircularList(CircularList &&source) // move constructor: clear the source before getting the new instance
    {
        s = source.s;
        cap = source.cap;
        storage = source.storage;
        source.storage = nullptr;
        head = source.head;
        tail = source.tail;
    }
    CircularList operator=(const CircularList &source) // overload =: y chang copy luon
    {
        if (storage != nullptr)
        {
            delete [] storage;
        }
        s = source.s;
        cap = source.cap;
        storage = new T[cap];
        head = source.head;
        tail = source.tail;
        for (int i = 0; i < s; ++i)
        {
            *(storage + (head + i) % cap) = *(source.storage + (head + i) % cap);
        }
        return *this;
    }

    T &operator[](int index)
    {
        return *(storage + (head + index) % cap);
    }
    T operator[](int index) const
    {
        if (index > s - 1)
        {
            return 0;
        }
        return *(storage + (head + index) % cap);
    }
        CircularList operator*(double factor) // overload =: y chang copy luon
    {
        CircularList temp = *this;
        for (int i = 0; i < s; ++i)
        {
           temp[i]= (*this)[i]*factor;
        }
        return temp;
    }
    T front()
    {
        return *(storage + head);
    }
    T back()
    {
        return *(storage + (head + size() - 1) % cap);
    }
    void push_front_and_pop_back(T data)
    {
        if (size() < cap)
        {
            push_front(data);
            pop_back();
        }
        else
        {
            pop_back();
            push_front(data);
        }
    }
    void push_back(T data)
    {

        s++;
        if (s > cap)
        {
            makeNewOnHeap();
        }
        *(storage + (head + s - 1) % cap) = data;
    }
    void pop_back()
    {

        s--;
        if (s < cap / 2 + 1)
        {

            tempStorage = storage;
            storage = new T[cap / 2];
            for (int i = 0; i < cap / 2; ++i)
            {
                *(storage + i) = *(tempStorage + (head + i) % cap);
            }
            head = 0;
            delete[] tempStorage;
            cap /= 2;
        }
    }
    void clear()
    {
        s = 0;
        cap = 0;

        delete[] storage;
        storage = nullptr;
    }
    void insert(int index, T data)
    {
        if (index == 0)
        {
            push_front(data);
        }
        else
        {

            s++;
            if (s > cap)
            {

                tempStorage = storage;
                storage = new T[s * 2];
                int offset = 0;
                for (int i = 0; i < cap; ++i)
                {
                    if (i == index)
                    {
                        *(storage + (head + i) % cap) = data;
                        offset = 1;
                    }
                    *(storage + (head + i) % cap + offset) = *(tempStorage + (head + i) % cap);
                }
                delete[] tempStorage;
                cap = s * 2;
            }
            else
            {
                for (int i = s - 1; i > index; --i)
                {
                    *(storage + (head + i) % cap) = *(storage + (head + i) % cap - 1);
                }
                *(storage + (head + index) % cap) = data;
            }
        }
    }
    void erase(int index)
    {
        for (int i = index; i < s - 1; ++i)
        {
            *(storage + (head + i) % cap) = *(storage + (head + i) % cap + 1);
        }
        s--;
        if (s < cap / 2 + 1)
        {

            tempStorage = storage;
            storage = new T[cap / 2];
            for (int i = 0; i < cap / 2; ++i)
            {
                *(storage + i) = *(tempStorage + (head + i) % cap);
            }
            head = 0;
            delete[] tempStorage;
            cap /= 2;
        }
    }
    void push_front(T data)
    {
        s++;
        if (s > cap)
        {
            makeNewOnHeap();
        }
        head = ((head - 1) + cap) % cap;
        *(storage + head) = data;
    }
    void pop_front(T data)
    {
        s--;
        head = (head + 1) % cap;
    }
    bool empty()
    {
        return (storage == nullptr) ? true : false;
    }
    int size() const
    {
        return s;
    }
private:
    friend ostream & operator<<(ostream &out, CircularList<T> value)
    {
        out<<"[";
        for (int i = 0; i < value.size()-1; i++)
            out<<value[i]<<",";
        out<<value[value.size()-1]<<"]";
        return out;
    }
};
