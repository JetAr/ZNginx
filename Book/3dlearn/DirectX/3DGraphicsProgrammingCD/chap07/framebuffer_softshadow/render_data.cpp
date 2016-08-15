#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// model object
CGutModel g_Model;
// camera control object
CGutUserControl g_Control;
// light info
Light g_Light;
Vector4 g_vAmbientLight(0.3f, 0.3f, 0.3f, 1.0f);
// camera fov
float g_fFOV = 45.0f;
// wall 
float wall_z = -2.0f;
const float wall_size_half = 4.0f;

Vertex_VT g_Quad[4] =
{
	{Vector4(-wall_size_half, -wall_size_half, wall_z), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( wall_size_half, -wall_size_half, wall_z), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-wall_size_half,  wall_size_half, wall_z), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( wall_size_half,  wall_size_half, wall_z), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};

// full screen quad
Vertex_VT g_FullScreenQuad[4] =
{
	{Vector4(-1.0f, -1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( 1.0f, -1.0f, 1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-1.0f,  1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( 1.0f,  1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};

bool g_bBlur = true;