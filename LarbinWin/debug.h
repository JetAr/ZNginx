#ifndef DEBUG_H_
#define DEBUG_H_

#ifndef NDEBUG

uint debUrl;
#define newUrl() debUrl++
#define delUrl() debUrl--
#else

#define newUrl() ((void) 0)
#define delUrl() ((void) 0)
#endif
#endif