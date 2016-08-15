#include "gut.h"
#include "render_data.h"

// 1 = no antialiasing, 2 = multi-sample, 3 = super-sample
int g_iMode = 1;
//
bool g_bWireframe = false;

Matrix4x4 g_world_matrix;

// triangle
Vector4 g_Triangle[3] = 
{
	Vector4( 0.0f,  0.577f, 0.0f, 1.0f),
	Vector4(-0.5f, -0.288f, 0.0f, 1.0f),
	Vector4( 0.5f, -0.288f, 0.0f, 1.0f),
};
// fullscreen quad
Vertex_VT g_Quad[] =
{
	{-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
	{ 1.0f, -1.0f, 0.0f, 1.0f, 1.0f},
	{-1.0f,  1.0f, 0.0f, 0.0f, 0.0f},
	{ 1.0f,  1.0f, 0.0f, 1.0f, 0.0f},
};
