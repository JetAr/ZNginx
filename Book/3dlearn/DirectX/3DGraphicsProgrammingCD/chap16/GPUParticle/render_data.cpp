#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"
#include "GutUserControl.h"

int g_iMode = 0x01|0x02|0x04|0x08;
//
sParticle *g_pParticleArray = NULL;
// user control object
CGutUserControl g_Control;
// camera settings
float g_fFOV = 45.0f;
float g_fCameraNearZ = 0.1f;
float g_fCameraFarZ = 100.0f;
//
float g_fEmitTheta = 15.0f;
float g_fEmitRate = g_texture_width * 2.0f;
//
float g_fTimeAdvance = 0.0f;
float g_fTimeElapsed = 0.0f;
//
int g_num_particles = 0;
//
Vector4 g_vForce(0.0f, -1.0f, 0.0f, 0.0f);

