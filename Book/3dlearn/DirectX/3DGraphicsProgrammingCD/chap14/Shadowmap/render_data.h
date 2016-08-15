#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

//
const int shadowmap_size = 1024;
// 
extern CGutUserControl g_Control;
extern CGutModel g_Model;
// camera fov
extern float g_fFOV;
// spotlight fov
extern float g_fLightFOV;
extern float g_fLightNearZ, g_fLightFarZ;
extern Vector4 g_vLightPos;
extern Vector4 g_vLightLookAt;
extern Vector4 g_vLightUp;
// 
extern float g_fZBias;