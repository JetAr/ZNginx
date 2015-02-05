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

    //返回下一个出堆的元素 内联函数
    T* read ()
    {
        return tab[out];
    }

    //返回下一个出队的元素，首先检查队列是否为空,为空返回NULL
    T* tryRead ();

    // 出队，若队列为空调用assert抛出异常，否则出队
    T *get ();

    //出队，若队列为空返回null，否则出队
    T* tryGet ();

    //入队，向队列添加一个新的元素。若队列已满则扩大队列容量为原来2倍
    void put (T *obj);

    //入队，仅用于将刚刚出队的元素再添加到队列中原来位置
    void rePut (T *obj);

    //返回队列长度
    int getLength ();

    //内联函数，判断队列是否为空
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