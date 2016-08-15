#ifndef _UTILITY_H_
#define _UTILITY_H_

UINT32 GenerateChecksum(void *data, int len);
UINT32 GenerateChecksum(const char *str);
unsigned short GenerateHash_16bits(void *data, int len);

#endif // _UTILITY_H_