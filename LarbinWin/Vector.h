
#ifndef VECTOR_H_
#define VECTOR_H_

#include "types.h"
#include <assert.h>

template<class>
class Vector
{
private:
    /**this array contain the object*/
    T **tab;
    /**number of objects in the array*/
    uint pos;
    /**siez of the array*/
    uint size;

public:
    Vector(uint size = StdVectSize);
    ~Vector();
    /** Re-init this vector : empty it all */
    void recycle ();
    void addElement (T *elt);
    uint getLength ()
    {
        return pos;
    }
    T **getTab ()
    {
        return tab;
    }
    T *operator[] (uint i);
};

template <class T>
Vector<T>::Vector(uint size)
{
    this->size = size;
    pos = 0;
    tab = new T*[size];
}

template <class T>
Vector<T>::~Vector ()
{
    for (uint i=0; i<pos; i++)
    {
        delete tab[i];
    }
    delete [] tab;
}

template <class T>
Vector<T>::recycle()
{
    for (uint i=0; i<pos; i++)
    {
        delete tab[i];
    }
    pos = 0;
}

template <class T>
Vector<T>::addElement (T *elt)
{
    assert(pos <= size);
    if (pos == size)
    {
        size = 2 * size;
        T **tmp = new T*[size];
        for(uint i=0; i<pos; i++)
        {
            tmp[i] = tab[i];
        }
        delete [] tab;
        tab = tmp;
    }
    tab[pos] = elt;
    pos++;
}

template <class T>
T *Vector<T>::operator [] (uint i)
{
    if (i<pos)
    {
        return tab[i];
    }
    else
        return NULL;
}
