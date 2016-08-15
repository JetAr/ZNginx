#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// 矩形的4個頂點
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{{-2.0f, 0.0f, -2.0f}, {0.0f, 4.0f}},
	{{ 2.0f, 0.0f, -2.0f}, {4.0f, 4.0f}},
	{{-2.0f, 0.0f, 2.0f}, {0.0f, 0.0f}},
	{{ 2.0f, 0.0f, 2.0f}, {4.0f, 0.0f}}
};

int g_iFilterMode = 2;

// controller object
CGutUserControl g_Control;
