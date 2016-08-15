#include "Vector4.h"
#include "render_data.h"

// 金字塔形的5個頂點位置及顏色的資料
Vertex_VC g_pyramid_vertices[5] = 
{
	{
		{ 0.0f, 1.0f, 0.0f}, // top
		{ 255, 255, 0, 255},
	},
	{
		{-0.5f, 0.0f, 0.5f},
		{ 255, 0, 0, 255},
		},
		{
			{ 0.5f, 0.0f, 0.5f},
			{ 255, 0, 0, 255},
		},
		{
			{ 0.5f, 0.0f,-0.5f},
			{ 255, 0, 0, 255},
			},
			{
				{-0.5f, 0.0f,-0.5f},
				{ 255, 0, 0, 255},
			}
};

// 組成金字塔側面的4個三角形的頂點索引值
unsigned short g_pyramid_trianglefan_indices[6] =
{
	0,
	1, 2, // triangle 1
	3,	  // triangle 2
	4,	  // triangle 3
	1	  // triangle 4
};

// 組成金字塔的6個三角形的頂點索引值
unsigned short g_pyramid_trianglelist_indices[12] =
{
	0, 1, 2, // triangle 1　
	0, 2, 3, // triangle 2
	0, 3, 4, // triangle 3
	0, 4, 1, // triangle 4
};


Vertex_VC g_road_vertices[4] =
{
	{
		{-0.5f, 0.0f, 0.5f},
		{255, 128, 0, 255},
	},
	{
		{ 0.5f, 0.0f, 0.5f},
		{255, 128, 0, 255},
		},
		{
			{ 0.5f, 0.0f,-0.5f},
			{255, 128, 0, 255},
		},
		{
			{-0.5f, 0.0f,-0.5f},
			{255, 128, 0, 255},
			}
};

unsigned short g_road_trianglestrip_indices[4] =
{
	0, 1, 3, 2
};


// 鏡頭位置
Vector4 g_eye(0.0f, 0.5f, 10.0f); 
// 鏡頭對準的點
Vector4 g_lookat; 
// 鏡頭正上方的方向
Vector4 g_up; 

Matrix4x4 g_view_matrix;

char g_map[8][8] = 
{
	{4, 0, 3, 0, 2, 0, 4, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{5, 0, 1, 0, 6, 0, 3, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{3, 0, 4, 0, 3, 0, 6, 0},
	{0, 0, 0, 0, 0, 0, 0, 0},
	{2, 0, 3, 0, 5, 0, 5, 0},
	{0, 0, 0, 0, 0, 0, 0, 0}
};
