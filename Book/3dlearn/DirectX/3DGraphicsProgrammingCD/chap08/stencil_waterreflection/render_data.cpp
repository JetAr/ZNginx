#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// model object
CGutModel g_Model;
// user control object
CGutUserControl g_Control;
// camera fov
float g_fFOV = 45.0f;
// on/off
bool g_bClip = true;
// mirror poly
float g_mirror_z = 0.0f;
const float mirror_size_half = 2.0f;

Vertex_VT g_Quad[4] =
{
	{Vector4(-mirror_size_half, -mirror_size_half, g_mirror_z), Vector4(0.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4( mirror_size_half, -mirror_size_half, g_mirror_z), Vector4(1.0f, 1.0f, 0.0f, 0.0f)},
	{Vector4(-mirror_size_half,  mirror_size_half, g_mirror_z), Vector4(0.0f, 0.0f, 0.0f, 0.0f)},
	{Vector4( mirror_size_half,  mirror_size_half, g_mirror_z), Vector4(1.0f, 0.0f, 0.0f, 0.0f)}
};
