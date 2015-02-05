#ifndef CHECKER_H_
#define CHECKER_H_

#include "types.h"
#include "Vector.h"

/** check if an url is allready known
 * if not send it
 * @param u the url to check
 */
void check (url *u);

/** Check the extension of an url
 * @return true if it might be interesting, false otherwise
 */
bool filter1 (char *host, char *file);

#endif
