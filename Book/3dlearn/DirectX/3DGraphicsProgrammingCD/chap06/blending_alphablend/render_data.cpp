#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// controller object
CGutUserControl g_Control;
// 矩形的4個頂點
// Position, Texcoord
Vertex_VCT g_Quad[4] =
{
	{{-1.0f, -1.0f, 0.0f}, {255,255,255,128}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f}, {255,255,255,128}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f}, {255,255,255,128}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f}, {255,255,255,128}, {1.0f, 0.0f}}
};

Vertex_VCT g_FullScreenQuad[4] =
{
	{{-1.0f, -1.0f, 1.0f}, {255,255,255,255}, {0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 1.0f}, {255,255,255,255}, {1.0f, 1.0f}},
	{{-1.0f,  1.0f, 1.0f}, {255,255,255,255}, {0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 1.0f}, {255,255,255,255}, {1.0f, 0.0f}}
};

int g_iMode = 1;