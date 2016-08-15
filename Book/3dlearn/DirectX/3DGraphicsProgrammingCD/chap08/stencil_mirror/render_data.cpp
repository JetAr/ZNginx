#include "Vector4.h"
#include "render_data.h"
#include "gut.h"

// model object
CGutModel g_Model;

// controller object
CGutUserControl g_Control;

// camera fov
float g_fFOV = 45.0f;

// mirror position & size
float g_mirror_z = -2.0f;
const float mirror_size_half = 2.0f;

// mirror poly
Vertex_V g_Quad_v[4] =
{
	{-mirror_size_half, -mirror_size_half, g_mirror_z},
	{ mirror_size_half, -mirror_size_half, g_mirror_z},
	{-mirror_size_half,  mirror_size_half, g_mirror_z},
	{ mirror_size_half,  mirror_size_half, g_mirror_z}
};
