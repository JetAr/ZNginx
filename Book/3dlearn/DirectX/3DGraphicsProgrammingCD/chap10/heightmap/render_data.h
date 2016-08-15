#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

//
extern bool g_bWireframe;
// controller object
extern CGutUserControl g_Control;
// 常數定義
const float g_fFovW = 45.0f;
// 光源資料
#define NUM_LIGHTS 1

struct Light
{
	Vector4 m_Position;
	Vector4 m_Direction;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
};

extern float g_fMaterialShininess;
extern Vector4 g_vAmbientLight;
extern Light g_Lights[NUM_LIGHTS];
