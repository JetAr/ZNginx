#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"
#include "GutModel.h"

struct Vertex_VC
{
	float m_Position[3];
	// R,G,B,A in OpenGL
	// B,G,R,A in Direct3D 9
	// Direct3D 10可是是兩種其中之1, 看設定而定.
	unsigned char m_RGBA[4];
};

struct Light
{
	Vector4 m_Position;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};

// model object
extern CGutModel g_Models[3];

extern Light g_Light;
extern Vector4 g_vAmbientLight;
// controller object
extern CGutUserControl g_Control;
// 邊線
extern Vertex_VC g_Border[4];
// 
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
