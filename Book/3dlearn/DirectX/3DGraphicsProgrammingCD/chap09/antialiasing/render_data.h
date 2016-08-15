#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VT
{
	float m_Position[3];
	float m_Tex[2];
};

// field of view
extern float g_fFOV;
// wireframe on/off
extern bool g_bWireframe;
// antialiasing mode
extern int g_iMode;
// 
extern Vector4 g_Triangle[];
//
extern Vertex_VT g_Quad[];
//
extern Matrix4x4 g_world_matrix;