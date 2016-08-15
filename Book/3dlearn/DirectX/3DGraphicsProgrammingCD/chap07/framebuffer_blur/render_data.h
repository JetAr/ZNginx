#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"
#include "GutDefs.h"

// blur
extern bool g_bBlur;
// model object
extern CGutModel g_Models[4];
// controller object
extern CGutUserControl g_Control;
// 
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
//
extern Vertex_V3T2 g_Quad[4];
