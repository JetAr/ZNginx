#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

struct Vertex_VT
{
	Vector4 m_Position; // 頂點位置
	Vector4 m_Texcoord; // 貼圖座標
};

// model object
extern CGutModel g_Model;
// controller object
extern CGutUserControl g_Control;
// camera fov
extern float g_fFOV;
// mirror
extern float g_mirror_z;
// 水面
extern Vertex_VT g_Quad[4];
// clip on/off
extern bool g_bClip;