#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// model object
CGutModel g_Model;
// controller object
CGutUserControl g_Control;
// camera field of view
float g_fFOV = 45.0f;
// full screen quad
Vertex_V g_Quad[4] =
{
	Vector4(-1.0f, -1.0f, 0.0f),
	Vector4( 1.0f, -1.0f, 0.0f),
	Vector4(-1.0f,  1.0f, 0.0f),
	Vector4( 1.0f,  1.0f, 0.0f)
};
