#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// camera object
CGutUserControl g_Control;
// model object
CGutModel g_Model;
CGutModel g_Model_textured;
// shadowvolume object
CGutShadowVolume g_ShadowVolume;
// camera fov
float g_fFOV = 45.0f;
//
const float fQuadSize = 4.0f;
const float fQuadZ = -2.0f;
//
bool g_bDrawShadowVolume = false;
bool g_bDirectionalLight = true;

Vertex_VT g_Quad[4] =
{
	{Vector4(-fQuadSize, -fQuadSize, fQuadZ), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( fQuadSize, -fQuadSize, fQuadZ), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-fQuadSize,  fQuadSize, fQuadZ), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( fQuadSize,  fQuadSize, fQuadZ), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};

Vertex_VT g_FullScreenQuad[4] =
{
	{Vector4(-1, -1, 0), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( 1, -1, 0), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-1,  1, 0), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( 1,  1, 0), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};
