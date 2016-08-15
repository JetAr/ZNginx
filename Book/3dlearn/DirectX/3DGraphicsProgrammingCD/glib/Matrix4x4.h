#ifndef _MATRIX4X4_
#define _MATRIX4X4_

#include "Vector4.h"

// General CPU Version, reference code
#include "Matrix4x4_Reference.h" 
// Intel SSE instruction version
#include "Matrix4x4_SSE.h" 

#ifdef _GENERAL_CPU_
typedef Matrix4x4CPU Matrix4x4;
#else
typedef Matrix4x4SSE Matrix4x4;
#endif

#endif