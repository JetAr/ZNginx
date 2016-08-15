#include "Vector4.h"

// 金字塔形的5個頂點
Vector4 g_vertices[5] = 
{
	Vector4(-1.0f, 1.0f, 0.0f),
	Vector4(-1.0f,-1.0f, 0.0f),
	Vector4( 1.0f,-1.0f, 0.0f),
	Vector4( 1.0f, 1.0f, 0.0f),
	Vector4( 0.0f, 0.0f, 1.0f),
};

// 連結出金字塔8條邊線的索引值
unsigned short g_indices[16] =
{
	0, 1,
	1, 2,
	2, 3,
	3, 0,
	0, 4,
	1, 4,
	2, 4,
	3, 4
};

// 4個金字塔的位移
Vector4 g_position[4] = 
{
	Vector4(-2.0f,-2.0f, 0.0f),
	Vector4( 2.0f,-2.0f, 0.0f),
	Vector4(-2.0f, 2.0f, 0.0f),
	Vector4( 2.0f, 2.0f, 0.0f),
};
// 4個金字塔的縮放值
Vector4 g_scale[4] = 
{
	Vector4( 1.0f, 1.0f, 1.0f),
	Vector4( 1.0f, 1.5f, 4.0f),
	Vector4( 1.0f, 1.0f, 2.0f),
	Vector4( 1.5f, 1.5f, 3.0f),
};

// 鏡頭位置
Vector4 g_eye(0.0f, 8.0f, 1.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 1.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, 0.0f, 1.0f); 
