/* This modules is a filter
 * given some normalized URLs, it makes sure their extensions are OK
 * and send them if it didn't see them before
 */

#include <iostream>
#include <string>

#include "options.h"

#include "types.h"
#include "global.h"
#include "url.h"
#include "text.h"
#include "Vector.h"
#include "hashTable.h"
#include "file.h"

#include "debug.h"

/** check if an url is allready known
 * if not send it
 * @param u the url to check
 */
void check (url *u)
{

}
