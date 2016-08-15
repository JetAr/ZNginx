#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// model object
CGutModel g_Model;
// controller object
CGutUserControl g_Control;
// 鏡頭位置
Vector4 g_eye(7.0f, 0.0f, 7.0f, 1.0f); 
// Camera FOV
float g_fFOV = 45.0f;
// mirror 
float g_mirror_z = -2.0f;
const float mirror_size_half = 2.0f;

Vertex_V g_Quad[4] =
{
	Vector4(-mirror_size_half, -mirror_size_half, g_mirror_z),
	Vector4( mirror_size_half, -mirror_size_half, g_mirror_z),
	Vector4(-mirror_size_half,  mirror_size_half, g_mirror_z),
	Vector4( mirror_size_half,  mirror_size_half, g_mirror_z)
};
