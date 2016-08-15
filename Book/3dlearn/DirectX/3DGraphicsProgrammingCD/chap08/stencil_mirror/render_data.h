#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

struct Vertex_V
{
	float m_Position[3]; // 頂點位置
	unsigned char m_Color[4]; // 顏色
};

// model object
extern CGutModel g_Model;
// controller object
extern CGutUserControl g_Control;
// camera fov
extern float g_fFOV;
// mirror position
extern float g_mirror_z;
// 矩形
extern Vertex_V  g_Quad_v[4];
