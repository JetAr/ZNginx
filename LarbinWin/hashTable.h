//larbin
//2012-05-31

/* class hashTable
   this class is in charge of making sure we don't crawl twice the same url

*/

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "types.h"
#include "url.h"

class hashTable
{
private:
    ssize_t size;
    char *table;
public:
    hashTable(bool create);
    ~hashTable();
    //��hashTable���浽һ���ļ���
    void save();
    //�ж�һ��url�Ƿ��Ѿ����������hashTable��
    bool test(url *U);
    //��url��hashTable�б��Ϊ�Ѿ�����
    void set(url *U);
    //�ж�url��û�б��ӹ�
    bool testSet(url *U);
};

#endif