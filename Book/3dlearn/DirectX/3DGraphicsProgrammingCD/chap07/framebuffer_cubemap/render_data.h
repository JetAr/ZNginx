#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutModel.h"
#include "GutUserControl.h"

// camera fox
extern float g_fFOV;
// model object
extern CGutModel g_Models[4];
// controller object
extern CGutUserControl g_Control;
extern float g_mirror_z;
// 更新cubemap時的鏡頭位置
extern Vector4 g_vCubemap_Eye;
extern Vector4 g_vCubemap_Lookat[6];
extern Vector4 g_vCubemap_Up[6];
// 鏡頭位置
extern Vector4 g_eye; 
// 鏡頭對準的點
extern Vector4 g_lookat; 
// 鏡頭正上方的方向
extern Vector4 g_up; 
// 鏡頭轉換矩陣
extern Matrix4x4 g_view_matrix;
extern Matrix4x4 g_scale_matrix;
extern Matrix4x4 g_world_matrix;
// 
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix, g_mars_matrix;
