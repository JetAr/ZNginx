#include "Vector4.h"
#include "render_data.h"
#include "gut.h"
#include "GutModel.h"

// 鏡頭位置
Vector4 g_eye(0.0f, 0.0f, 30.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, 1.0f, 0.0f); 
// 鏡頭轉換矩陣
Matrix4x4 g_view_matrix;
// 物件旋轉矩陣
Matrix4x4 g_world_matrix;
// 讀取模型的物件
CGutModel g_Model;

// 轉換頂點資料的函式, Direct3D跟OpenGL可以共用.
void CopyVertexBuffer(void *target, sModelVertex *source, int num_vertices)
{
	// 這個范例中, OpenGL跟Direct3D執行時只需要Position & Normal
	struct sRunTimeVertex
	{
		float m_Position[3];
		float m_Normal[3];
	};

	sRunTimeVertex *pVertexArray = (sRunTimeVertex *) target;

	for ( int v=0; v<num_vertices; v++ )
	{
		sRunTimeVertex *p = pVertexArray + v;

		p->m_Position[0] = source[v].m_Position[0];
		p->m_Position[1] = source[v].m_Position[1];
		p->m_Position[2] = source[v].m_Position[2];

		p->m_Normal[0] = source[v].m_Normal[0];
		p->m_Normal[1] = source[v].m_Normal[1];
		p->m_Normal[2] = source[v].m_Normal[2];
	}
}
