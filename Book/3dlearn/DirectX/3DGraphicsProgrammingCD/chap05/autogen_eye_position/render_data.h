#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"

struct Vertex
{
	float m_Position[3]; // 頂點位置
};

// 常數定義
//const float g_fFovW = 45.0f;
const float g_fFovW = 90.0f;
// Vertex
extern Vertex g_Quad[4];
// controller object
extern CGutUserControl g_Control;
