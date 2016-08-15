#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"
#include "GutUserControl.h"

// user control object
CGutUserControl g_Control;
// teapot model
CGutModel g_Model;
// spot light model
CGutModel g_SpotLightModel;
// camera fov
float g_fFOV = 45.0f;
