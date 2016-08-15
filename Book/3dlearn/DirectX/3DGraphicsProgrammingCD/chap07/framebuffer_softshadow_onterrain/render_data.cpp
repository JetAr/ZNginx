#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"
#include "GutUserControl.h"

// models
CGutModel g_Model;
CGutModel g_Terrain;
// camera control object
CGutUserControl g_Control;
// lighting
Light g_Light;
Vector4 g_vAmbientLight(0.3f, 0.3f, 0.3f, 1.0f);
// camera fov
float g_fFOV = 60.0f;
// full screen quad
Vertex_VT g_Quad[4] =
{
	{Vector4(-1.0f, -1.0f, 1.0f), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( 1.0f, -1.0f, 1.0f), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-1.0f,  1.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( 1.0f,  1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};
