//Larbin

/* class hashDup
 * This class id in charge of making sure we don't crawl twice the same page
 */

#ifndef HASHDUP_H_
#define HASHDUP_H_
#include "types.h"
class hashDup
{
private:
    ssize_t size;
    char *table;
    char *file;
public:
//    constructor
    hashDup(ssize_t size, char *init, bool scratch);
//    destructer
    ~hashDup();

    /* set a page in the hashtable
     * return false if it was already there
     * return true if it was not (ie it is new)
     */
    bool testSet (char *doc);
//   save in a file
    void save ();
};
#endif  //HASHDUP_H_
