#include "Vector4.h"
#include "Matrix4x4.h"

struct Vertex_VC
{
	float m_Position[3];
	// R,G,B,A in OpenGL
	// B,G,R,A in Direct3D 9
	// Direct3D 10可是是兩種其中之1, 看設定而定.
	unsigned char m_RGBA[4];
};

// 金字塔形的8條邊線
extern Vertex_VC g_pyramid_vertices[5];
// 
extern unsigned short g_pyramid_trianglefan_indices[6];

extern unsigned short g_pyramid_trianglelist_indices[12];

extern Vertex_VC g_road_vertices[4];

extern unsigned short g_road_trianglestrip_indices[4];

// 地圖
extern char g_map[8][8];


// 鏡頭位置
extern Vector4 g_eye; 
// 鏡頭對準的點
extern Vector4 g_lookat; 
// 鏡頭正上方的方向
extern Vector4 g_up; 

//
extern Matrix4x4 g_object_matrix;