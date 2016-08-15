#ifndef __ElLight_H__
#define __ElLight_H__

#include "ElD3DHeaders.h"
#include <vector>

typedef std::vector<D3DLIGHT9> ElLightList;
typedef std::vector<D3DLIGHT9>::iterator ElLightIterator;
typedef std::vector<D3DLIGHT9>::const_iterator ElLightConstIterator;

#endif //__ElLight_H__