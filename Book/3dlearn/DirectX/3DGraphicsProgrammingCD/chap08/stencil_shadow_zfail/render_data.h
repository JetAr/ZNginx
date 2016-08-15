#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutShadowVolume.h"
#include "GutUserControl.h"

struct Vertex_VT
{
	Vector4 m_Position; // 頂點位置
	Vector4 m_Texcoord; // 貼圖座標
};

// camera control object
extern CGutUserControl g_Control;
// model object
extern CGutModel g_Model;
extern CGutModel g_Model_textured;
// shadow volume
extern CGutShadowVolume g_ShadowVolume;
//
extern bool g_bDrawShadowVolume;
//
extern bool g_bDirectionalLight;
// camera fov
extern float g_fFOV;
// 矩形
extern Vertex_VT g_Quad[4];
extern Vertex_VT g_FullScreenQuad[4];
