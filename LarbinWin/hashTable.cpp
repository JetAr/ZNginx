#include <fstream>
#include <iostream>

#include "hashTable.h"
#include "types.h"
//���캯��
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
                fin.read(table+sr,total-sr);   //ֱ��һ��read�᲻����������أ�
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

//��������
hashTable::~hashTable()
{
    delete [] table;
}

//��hashTable���浽һ���ļ���
void hashTable::save()
{
    std::rename("hashTable.bak","hashTable.old");
    std::ofstream fout;
    fout.open("hashTable.bak",std::ios::binary|std::ios::out);
    fout.write(table, hashSize/8);
    fout.close();
    std::remove("hashTable.old");
}

//�ж�һ��url�Ƿ��Ѿ����������hashTable��
bool hashTable::test(url *U)
{
    unsigned int code = U->hashCode();
    unsigned int pos = code /8;
    unsigned int bits = 1 <<(code%8);
    return table[pos]&bits;
}

//��url��hashTable�б��Ϊ�Ѿ�����
void hashTable::set(url *U)
{
    unsigned int code = U->hashCode();
    unsigned int pos = code /8;
    unsigned int bits = 1 <<(code%8);
    table[pos] |= bits;

}
//�ж�url��û�б��ӹ�,�Ѿ���������false��û�м���������Ϊ�Ѿ�����
bool hashTable::testSet(url *U)
{
    unsigned int code = U->hashCode();
    unsigned int pos = code / 8;
    unsigned int bits = 1 << (code % 8);
    int res = table[pos] & bits;
    table[pos] |= bits;
    return !res;

}
