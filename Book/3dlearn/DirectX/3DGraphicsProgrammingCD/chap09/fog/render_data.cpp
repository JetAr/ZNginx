#include "gut.h"
#include "render_data.h"

// model object
CGutModel g_Model;
// controller object
CGutUserControl g_Control;
// camera field of view
float g_fFOV = 60.0f;
// 1=linear fog, 2 = exp fog, 3 = exp2 fog
int g_iFogMode = 1;