#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// model object
CGutModel g_Model;
// user control object
CGutUserControl g_Control;
// light info
Light g_Light;
Vector4 g_vAmbientLight(0.3f, 0.3f, 0.3f, 1.0f);
// camera fov
float g_fFOV = 45.0f;
// mirror
float g_mirror_z = -2.0f;
const float mirror_size_half = 4.0f;
// Position, Texcoord
Vertex_VT g_Quad[4] =
{
	{Vector4(-mirror_size_half, -mirror_size_half, g_mirror_z), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( mirror_size_half, -mirror_size_half, g_mirror_z), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-mirror_size_half,  mirror_size_half, g_mirror_z), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( mirror_size_half,  mirror_size_half, g_mirror_z), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};
