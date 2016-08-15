#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutShadowVolume.h"
#include "GutUserControl.h"

// camera control object
extern CGutUserControl g_Control;
// model object
extern CGutModel g_Box_shadowvolume;
extern CGutModel g_Box;
extern CGutModel g_Wall;
// shadow volume
extern CGutShadowVolume g_ShadowVolume;
//
extern bool g_bDrawShadowVolume;
//
extern bool g_bDirectionalLight;
// camera fov
extern float g_fFOV;
// 矩形
extern Vertex_V3T2 g_Quad[4];
extern Vertex_V3T2 g_FullScreenQuad[4];
