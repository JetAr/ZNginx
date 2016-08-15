#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

struct Vertex_VT
{
	Vector4 m_Position; // 頂點位置
	Vector4 m_Texcoord; // 貼圖座標
};

struct Light
{
	Vector4 m_Position;
	Vector4 m_Direction;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};
// controller object
extern CGutUserControl g_Control;
// models
extern CGutModel g_Model;
extern CGutModel g_Terrain;
// lighting
extern Light g_Light;
extern Vector4 g_vAmbientLight;
// camera fov
extern float g_fFOV;
// 矩形
extern Vertex_VT g_Quad[4];
