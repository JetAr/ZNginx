#include "Vector4.h"
#include "Matrix4x4.h"
#include "GutUserControl.h"
#include "GutModel.h"

// 鏡頭位置
extern Vector4 g_eye; 
// 鏡頭對準的點
extern Vector4 g_lookat; 
// 鏡頭正上方的方向
extern Vector4 g_up; 
// 鏡頭轉換矩陣
extern Matrix4x4 g_view_matrix;
extern Matrix4x4 g_world_matrix;
extern Matrix4x4 g_sun_matrix, g_earth_matrix, g_moon_matrix;
//
extern CGutModel g_sun_model, g_earth_model;
//
extern CGutUserControl g_Control;