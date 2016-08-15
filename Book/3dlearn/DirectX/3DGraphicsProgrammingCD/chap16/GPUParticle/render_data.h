#include "Vector4.h"
#include "Matrix4x4.h"

#include "GutDefs.h"
#include "GutModel.h"
#include "GutUserControl.h"

const int g_texture_width = 64;
const int g_texture_height = 1024;
const int g_max_particles = g_texture_width * g_texture_height;
extern int g_num_particles;

extern float g_fTimeAdvance;
extern float g_fTimeElapsed;
extern float g_fEmitRate;

extern int g_iMode;
//

extern float g_fEmitTheta;
//
extern Vector4 g_vForce;
// 
extern CGutUserControl g_Control;
// camera fov
extern float g_fFOV;
extern float g_fCameraNearZ, g_fCameraFarZ;
//
struct sParticle
{
	float m_Position[2];
};

extern sParticle *g_pParticleArray;