#ifndef TYPES_H_
#define TYPES_H_

// Max size for a url
#define maxUrlSize 512
#define maxSiteSize 40    // max size for the name of a site
#define maxCookieSize 128
#define namedSiteListSize 20000
#define hashSize 64000000

// Max number of urls per site in Url
#define maxUrlsBySite 40  // must fit in uint8_t

//standard types
typedef unsigned int uint;
#define ssize_t unsigned int
#endif