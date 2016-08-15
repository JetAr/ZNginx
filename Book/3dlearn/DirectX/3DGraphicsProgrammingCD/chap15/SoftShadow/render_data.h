#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

//
const int shadowmap_size = 512;
const int PCF_sample_span = 3;
const int PCF_samples_row = PCF_sample_span * 2 + 1;
const int PCF_samples = (PCF_samples_row * PCF_samples_row);

const int Interpolated_PCF_samples = 16;
extern int g_iMode;
// 
extern CGutUserControl g_Control;
extern CGutModel g_Model;
extern CGutModel g_Wall;
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
//
extern Vector4 g_vTexOffset[PCF_samples];
extern Vector4 g_vTexOffset2[PCF_samples];