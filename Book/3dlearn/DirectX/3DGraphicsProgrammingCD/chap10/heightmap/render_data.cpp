#include "Gut.h"
#include "GutHeightmap.h"
#include "render_data.h"

//
bool g_bWireframe = false;
// controller object
CGutUserControl g_Control;
// heightmap object
CGutHeightmap g_Heightmap;
// 光源資料
Vector4 g_vAmbientLight(0.1f, 0.1f, 0.1f, 0.1f);
// 材質
float g_fMaterialShininess = 60;
//
Light g_Lights[NUM_LIGHTS];
