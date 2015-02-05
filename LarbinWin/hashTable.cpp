#include <fstream>
#include <iostream>

#include "hashTable.h"
#include "types.h"
//构造函数
hashTable::hashTable(bool create)
{
    ssize_t total = hashSize/8;
    table = new char[total];
    if(!table)
        std::cerr << "alloc error";
    if(create)
    {
        for(ssize_t i=0; i<total; i++)
            table[i] = 1;
    }
    else
    {
        std::ifstream fin;

        fin.open("hashtable.bak",std::ios::in|std::ios::binary);
        if(fin.is_open())
        {
            ssize_t sr = 0;
            while(sr < total)
            {
                fin.read(table+sr,total-sr);   //直接一个read会不会出现问题呢？
            }
        }
        else
        {
            std::cerr <<"Can't find hashtable.bak, restart from scratch\n";
            for(ssize_t i=0; i<total; i++)
                table[i] = 0;
        }
        fin.close();

    }

}

//析构函数
hashTable::~hashTable()
{
    delete [] table;
}

//把hashTable保存到一个文件中
void hashTable::save()
{
    std::rename("hashTable.bak","hashTable.old");
    std::ofstream fout;
    fout.open("hashTable.bak",std::ios::binary|std::ios::out);
    fout.write(table, hashSize/8);
    fout.close();
    std::remove("hashTable.old");
}

//判断一个url是否已经存在于这个hashTable中
bool hashTable::test(url *U)
{
    unsigned int code = U->hashCode();
    unsigned int pos = code /8;
    unsigned int bits = 1 <<(code%8);
    return table[pos]&bits;
}

//将url在hashTable中标记为已经见过
void hashTable::set(url *U)
{
    unsigned int code = U->hashCode();
    unsigned int pos = code /8;
    unsigned int bits = 1 <<(code%8);
    table[pos] |= bits;

}
//判断url有没有被加过,已经见过返回false，没有见过将其标记为已经见过
bool hashTable::testSet(url *U)
{
    unsigned int code = U->hashCode();
    unsigned int pos = code / 8;
    unsigned int bits = 1 << (code % 8);
    int res = table[pos] & bits;
    table[pos] |= bits;
    return !res;

}
