#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	int num_vertices = sizeof(g_pyramid_vertices)/sizeof(Vertex_VC);

	// 把RGBA的顏色資料轉換成DX9 BGRA格式
	for ( int i=0; i<num_vertices; i++ )
	{
		unsigned char temp = g_pyramid_vertices[i].m_RGBA[0];
		g_pyramid_vertices[i].m_RGBA[0] = g_pyramid_vertices[i].m_RGBA[2];
		g_pyramid_vertices[i].m_RGBA[2] = temp;
	}

	num_vertices = sizeof(g_road_vertices)/sizeof(Vertex_VC);
	for ( int i=0; i<num_vertices; i++ )
	{
		unsigned char temp = g_road_vertices[i].m_RGBA[0];
		g_road_vertices[i].m_RGBA[0] = g_road_vertices[i].m_RGBA[2];
		g_road_vertices[i].m_RGBA[2] = temp;
	}

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// `清除畫面`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	// `開始下繪圖指令`
	device->BeginScene(); 

	// `設定資料格式`
	// `D3DFVF_XYZ = 使用3個浮點數來記錄位置`
	// `D3DFVF_DIFFUSE = 使用32bits整數型態來記錄BGRA顏色`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	Vector4 border(-15.0f, 0.0f, -15.0f);
	Vector4 grid_position = border;

	const int grids_x = 30;
	const int grids_z = 30;

	for ( int x=0; x<grids_x; x++ )
	{
		int grid_x = x & 0x07;
		grid_position[2] = border[2];

		for ( int z=0; z<grids_z; z++ )
		{
			int grid_z = z & 0x07;
			char c = g_map[grid_x][grid_z];

			// `設定轉換矩陣`
			Matrix4x4 object_matrix;
			object_matrix.Translate_Replace(grid_position);

			if ( c==0 ) // `馬路`
			{
				device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &object_matrix);
				// `畫出地板`
				device->DrawIndexedPrimitiveUP(
					D3DPT_TRIANGLESTRIP,
					0,
					4,
					2,
					g_road_trianglestrip_indices,
					D3DFMT_INDEX16,
					g_road_vertices,
					sizeof(Vertex_VC)
					);
			}
			else // `金字塔`
			{
				// `設定金字塔的高度`
				object_matrix.Scale_Replace(1.0f, (float) c, 1.0f);
				object_matrix[3] = grid_position;
				device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &object_matrix);
				// `畫出金字塔`
				device->DrawIndexedPrimitiveUP(
					D3DPT_TRIANGLEFAN, // `指定所要畫的基本圖形種類`
					0, // `會使用的最小頂點編號, 事實上沒太大用處.`
					5, // `頂點陣列里有幾個頂點`
					4, // `要畫出幾個基本圖形`
					g_pyramid_trianglefan_indices,	// `索引陣列`
					D3DFMT_INDEX16,		// `索引陣列的型態`
					g_pyramid_vertices,	// `頂點陣列`
					sizeof(Vertex_VC)	// `頂點陣列里每個頂點的記憶體間距`
					);
			}

			grid_position[2] += 1.0f;
		}

		grid_position[0] += 1.0f;
	}

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
