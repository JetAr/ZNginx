// Larbin

/* standard fifo in RAM WITHOUT synchronisation */


#ifndef FIFO_H_
#define FIFO_H_

#include "assert.h"

template <class T>
class Fifo
{
public:
    uint in;
    uint out;
    uint size;
    T **tab;


    Fifo(uint size = maxUrlsBySite);

    ~Fifo();

    //������һ�����ѵ�Ԫ�� ��������
    T* read ()
    {
        return tab[out];
    }

    //������һ�����ӵ�Ԫ�أ����ȼ������Ƿ�Ϊ��,Ϊ�շ���NULL
    T* tryRead ();

    // ���ӣ�������Ϊ�յ���assert�׳��쳣���������
    T *get ();

    //���ӣ�������Ϊ�շ���null���������
    T* tryGet ();

    //��ӣ���������һ���µ�Ԫ�ء������������������������Ϊԭ��2��
    void put (T *obj);

    //��ӣ������ڽ��ոճ��ӵ�Ԫ������ӵ�������ԭ��λ��
    void rePut (T *obj);

    //���ض��г���
    int getLength ();

    //�����������ж϶����Ƿ�Ϊ��
    bool isEmpty ()
    {
        return (in == out);
    }
};


template <class T>
Fifo<T>::Fifo(uint size)
{
    tab = new T*[size];
    this->size = size;
    in = out = 0;
}

template <class T>
Fifo<T>::~Fifo()
{
    delete [] tab;
}

template <class T>
T* Fifo<T>::tryRead()
{
    if(in == out)
        return NULL;
    return tab[out];
}

template <class T>
T* Fifo<T>::get()
{
    T *tmp;
    assert(in != out);
    tmp = tab[out];
    out = (out + 1)%size;
    return tmp;
}

template <class T>
T* Fifo<T>::tryGet()
{
    T *tmp;
    if (in == out) return NULL;
    tmp = tab[out];
    out = (out + 1)%size;
    return tmp;
}

template <class T>
void Fifo<T>::put(T* obj)
{
    tab[in] = obj;
    in = (in+1)%size;

    if(in == out)
    {
        T **tmp = new T*[2*size];
        for(uint i = out; i < size; i++)
        {
            tmp[i] = tab[i];
        }
        for(uint i = 0; i < in; i++)
        {
            tmp[i+size] = tab[i];
        }
        in += size;
        size = 2*size;
        delete [] tab;
        tab = tmp;
    }
}

template <class T>
void Fifo<T>::rePut(T* obj)
{
    out = (out + size - 1) % size;
    tab[out] = obj;
}

template <class T>
int Fifo<T>::getLength()
{
    return (in + size - out)%size;
}

#endif