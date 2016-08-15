#include <stdio.h>
#include <stdlib.h>

#include "Vector4.h"

Vector4CPU g_VectorCPU_Zero = Vector4CPU(0.0f, 0.0f, 0.0f, 0.0f);
Vector4CPU g_VectorCPU_One = Vector4CPU(1.0f, 1.0f, 1.0f, 1.0f);
Vector4CPU g_VectorCPU_MinusOne = Vector4CPU(-1.0f, -1.0f, -1.0f, -1.0f);
Vector4CPU g_VectorCPU_AbsMask = Vector4CPU( (unsigned int)0x7fffffff );
Vector4CPU g_VectorCPU_SignMask = Vector4CPU( (unsigned int)0x80000000 );

void Vector4CPU::ConsoleOutput(void)
{
	printf("Vector: (%5.2f,%5.2f,%5.2f,%5.2f)\n", x, y, z, w);
}
