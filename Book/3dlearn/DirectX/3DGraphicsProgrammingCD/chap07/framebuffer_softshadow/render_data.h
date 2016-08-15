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
// lighting info
extern Light g_Light;
extern Vector4 g_vAmbientLight;
// controller object
extern CGutUserControl g_Control;
// model object
extern CGutModel g_Model;
// camer afov
extern float g_fFOV;
// blur on/off
extern bool g_bBlur;
// 矩形
extern Vertex_VT g_Quad[4];
extern Vertex_VT g_FullScreenQuad[4];

