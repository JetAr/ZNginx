#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"
#include "GutUserControl.h"

int g_iMode = 1;
// user control object
CGutUserControl g_Control;
// teapot model
CGutModel g_Model;
CGutModel g_Wall;
// camera settings
float g_fFOV = 45.0f;
float g_fCameraNearZ = 0.1f;
float g_fCameraFarZ = 100.0f;
// light settings
float g_fLightFOV = 45.0f;
float g_fLightNearZ = 0.5f;
float g_fLightFarZ = 20.0f;

Vector4 g_vLightPos(0.0f, 0.0f, 10.0f);
Vector4 g_vLightLookAt(0.0f, 0.0f, 0.0f);
Vector4 g_vLightUp(0.0f, 1.0f, 0.0f);

float g_fZBias = 0.05f;

Vector4 g_vTexOffset[PCF_samples];
Vector4 g_vTexOffset2[PCF_samples];