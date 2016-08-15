#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// Position, Texcoord
Vertex_VT g_FullScreenQuad[4] =
{
	{{-1.0f, -1.0f, 0.0f},{0.0f, 1.0f}},
	{{ 1.0f, -1.0f, 0.0f},{1.0f, 1.0f}},
	{{-1.0f,  1.0f, 0.0f},{0.0f, 0.0f}},
	{{ 1.0f,  1.0f, 0.0f},{1.0f, 0.0f}}
};

int g_iPosteffect = 2;
float g_fSaturation = 0.5f;

Matrix4x4 g_SepiaMatrix(
						0.299f, 0.299f, 0.299f, 0.0f,
						0.587f, 0.587f, 0.587f, 0.0f,
						0.184f, 0.184f, 0.184f, 0.0f,
						80.0f/255.0f, 43.0f/255.0f, -23.0f/255.0f, 1.0f);

Matrix4x4 g_SaturateMatrix;