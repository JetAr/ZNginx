//Larbin

/* class hashDup
 * This class id in charge of making sure we don't crawl twice the same page
 */

#include <iostream>
#include <fstream>

#include "hashDup.h"
#include "types.h"

//    constructor
//     init 为文件名称 scratch判断是否重新开始
hashDup::hashDup(ssize_t size, char *init, bool scratch)
{
    this->size = size;
    file = init;
    table = new char[size / 8];
    if(init == NULL || scratch)
    {
        for(ssize_t i = 0; i < size/8; i++)
        {
            table[i] = 0;
        }
    }
    else
    {
        std::ifstream fin;
        fin.open(init, std::ios::binary | std::ios::in);
        if(!fin.is_open())
        {
            std::cerr << "Can not find " << init <<", restart from scratch";
            for(ssize_t i = 0; i < size/8; i++)
            {
                table[i] = 0;
            }
        }
        else
        {
            fin.read(table, size / 8);
        }
        fin.close();
    }
}


//    destructer
hashDup::~hashDup()
{
    delete [] table;
}

/* set a page in the hashtable
* return false if it was already there
* return true if it was not (ie it is new)
*/
bool hashDup::testSet (char *doc)
{
    unsigned int code = 0;
    char c;
    for(uint i = 0; (c = doc[i]) != 0; i++)
    {
        if (c >='A' && c <= 'z')
            code = (code *23 + c)%size;
    }
    unsigned int pos = code / 8;
    unsigned int bits = code % 8;
    int res = table[pos] & bits;
    table[pos] |= bits;
    return !res;
}
//   save in a file
void hashDup::save ()
{
    std::ofstream fout;
    fout.open(file,std::ios::out|std::ios::binary);
    if(fout.is_open())
    {
        fout.write(table, size / 8);
    }
    else
    {
        std::cerr << "cannot write files" << file << std::endl;
    }


}


