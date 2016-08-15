#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

struct CompressedVertex
{
	unsigned char m_Position[4];
	unsigned char m_Normal[4];
};

// controller object
extern CGutUserControl g_Control;
// 球形
extern CompressedVertex *g_pCompressedVertices;
extern unsigned short *g_pSphereIndices;
//
extern int g_iNumSphereVertices;
extern int g_iNumSphereTriangles;
extern int g_iNumSphereIndices;
// 
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
// 產生球模型的程式
bool CreateSphere(CompressedVertex **ppVertices, unsigned short **ppIndices, int stacks = 20, int pieces = 20);
