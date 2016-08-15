#include "Vector4.h"

// 金字塔形的8條邊線
extern Vector4 g_vertices[5];
extern unsigned short g_indices[16];
// 金字塔的位移
extern Vector4 g_position[4];
// 金字塔的大小
extern Vector4 g_scale[4];

// 鏡頭位置
extern Vector4 g_eye; 
// 鏡頭對準的點
extern Vector4 g_lookat; 
// 鏡頭正上方的方向
extern Vector4 g_up; 
