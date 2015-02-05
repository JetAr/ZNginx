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
    //把hashTable保存到一个文件中
    void save();
    //判断一个url是否已经存在于这个hashTable中
    bool test(url *U);
    //将url在hashTable中标记为已经见过
    void set(url *U);
    //判断url有没有被加过
    bool testSet(url *U);
};

#endif