#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutDefs.h"
#include "GutModel.h"
#include "GutUserControl.h"

struct Vertex_VC
{
	Vector4 m_Position;
	Vector4 m_Color;
};

#define NUM_LIGHTS 128

extern sGutLight g_Lights[NUM_LIGHTS];
extern Vertex_VC g_LightsVC[NUM_LIGHTS];

extern int g_iMode;
// 
extern CGutUserControl g_Control;
extern CGutModel g_Model;
extern CGutModel g_Sphere;

// camera fov
extern float g_fFOV;

// spotlight fov
extern float g_fLightFOV;
extern float g_fLightNearZ, g_fLightFarZ;

struct Vertex_VT
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};

extern Vertex_VT g_FullScreenQuad[4];
