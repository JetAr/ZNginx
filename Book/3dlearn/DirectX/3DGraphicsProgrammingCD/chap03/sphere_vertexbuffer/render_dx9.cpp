#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
static LPDIRECT3DINDEXBUFFER9 g_pIndexBuffer = NULL;

bool InitResourceDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `設定視角轉換矩陣`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// `關閉打光`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// `改變三角形正面的面向`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	for ( int i=0; i<g_iNumSphereVertices; i++ )
	{
		unsigned char temp = g_pSphereVertices[i].m_RGBA[0];
		g_pSphereVertices[i].m_RGBA[0] = g_pSphereVertices[i].m_RGBA[2];
		g_pSphereVertices[i].m_RGBA[2] = temp;
	}

	int vsize = g_iNumSphereVertices * sizeof(Vertex_VC);

	if ( D3D_OK!=device->CreateVertexBuffer(vsize, 0, 0, D3DPOOL_MANAGED, &g_pVertexBuffer, NULL) )
	{
		return false;
	}

	Vertex_VC *pVertexArray = NULL;
	// `取得Vertex Buffer的記憶體位置`
	g_pVertexBuffer->Lock(0, vsize, (void **)&pVertexArray, 0);
	memcpy(pVertexArray, g_pSphereVertices, vsize);
	g_pVertexBuffer->Unlock();

	int isize = g_iNumSphereIndices * sizeof(unsigned short);

	if ( D3D_OK!=device->CreateIndexBuffer(isize, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &g_pIndexBuffer, NULL) )
	{
		return false;
	}

	unsigned short *pIndexArray = NULL;
	// `取得Index Buffer的記憶體位置`
	g_pIndexBuffer->Lock(0, isize, (void **)&pIndexArray, 0);
	memcpy(pIndexArray, g_pSphereIndices, isize);
	g_pIndexBuffer->Unlock();

	return true;
}

bool ReleaseResourceDX9(void)
{
	if ( g_pVertexBuffer )
	{
		g_pVertexBuffer->Release();
		g_pVertexBuffer = NULL;
	}

	if ( g_pIndexBuffer )
	{
		g_pIndexBuffer->Release();
		g_pIndexBuffer = NULL;
	}

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	GutResetGraphicsDeviceDX9();

	// `設定視角轉換矩陣`
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);

	// `關閉打光`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// `改變三角形正面的面向`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	// `開始下繪圖指令`
	device->BeginScene(); 

	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// `設定資料格式`
	// D3DFVF_XYZ = `使用3個浮點數來記錄位置`
	// D3DFVF_DIFFUSE = `使用32bits整數型態來記錄BGRA顏色`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	device->SetStreamSource(0, g_pVertexBuffer, 0, sizeof(Vertex_VC));
	device->SetIndices(g_pIndexBuffer);

	// `畫出球`
	device->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST, // `指定所要畫的基本圖形種類`
		0, 
		0, // `會使用的最小頂點編號, 事實上沒太大用處.`
		g_iNumSphereVertices, // `頂點陣列里有幾個頂點`
		0, 
		g_iNumSphereTriangles // `要畫出幾個基本圖形`
		);

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
