#include <stdio.h>
#include "Gut.h"
#include "Vector4.h"

void RenderFrameDX9(void);
void RenderFrameOpenGL(void);

void main(int argc, char *argv[])
{
	// 內定使用DirectX 9來繪圖
	char *device = "dx9";

	if ( argc > 1 )
	{
		// 如果命令列參數指定用OpenGL, 就改用OpenGL.
		if ( stricmp(argv[1], "opengl")==0 )
		{
			device = "opengl";
		}
	}

	// 在(100,100)的位置開啟一個大小為(512x512)的視窗
	GutCreateWindow(100, 100, 512, 512, device);

	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		if ( !strcmp(device, "dx9") )
		{
			RenderFrameDX9();
		}
		else
		{
			RenderFrameOpenGL();
		}
	}

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}

// 金字塔形的8條邊線
Vector4 g_vertices[] = 
{
	Vector4(-1.0f, 1.0f,-1.0f),
	Vector4(-1.0f,-1.0f,-1.0f),

	Vector4(-1.0f,-1.0f,-1.0f),
	Vector4( 1.0f,-1.0f,-1.0f),

	Vector4( 1.0f,-1.0f,-1.0f),
	Vector4( 1.0f, 1.0f,-1.0f),

	Vector4( 1.0f, 1.0f,-1.0f),
	Vector4(-1.0f, 1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4(-1.0f, 1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4(-1.0f,-1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4( 1.0f,-1.0f,-1.0f),

	Vector4( 0.0f, 0.0f, 1.0f),
	Vector4( 1.0f, 1.0f,-1.0f),
};

// 鏡頭位置
Vector4 g_eye(0.0f,3.0f,3.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, -1.0f, 1.0f); 

// 使用DirectX9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DMATRIX pMatrix[3]={0};

	device->GetTransform(D3DTS_WORLD , &pMatrix[0] );
	device->GetTransform(D3DTS_VIEW , &pMatrix[1] );
	device->GetTransform(D3DTS_PROJECTION , &pMatrix[2] );

	//D3DXMatrixOrthoLH(&matProject, width, height, Znear, Zfar);


	device->SetRenderState( D3DRS_LIGHTING, FALSE );
	device->Clear(
		0, NULL, // 清除整個畫面
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // 清除顏色跟Z Buffer
		D3DCOLOR_ARGB(0, 0, 0, 0), // 設定要把顏色清成黑色
		1.0f, // 設定要把Z值清為1, 也就是離鏡頭最遠.
		0 // 設定要把Stencil buffer清為0, 在這沒差.
		);

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 設定鏡頭轉換矩陣
	// 因為記憶體排列方法相同, 可以直接把Matrix4x4轉型成D3DMATRIX.
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 計算出一個使用非平行投影的矩陣
	Matrix4x4 perspective_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 1.0f, 100.0f);
	// 設定視角轉換矩陣
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&perspective_matrix);

	// 開始下繪圖指令
	device->BeginScene();	
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZ);
	// 畫出金字塔的8條邊線
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, g_vertices, sizeof(Vector4)); 
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 

	// 呈現出背景backbuffer的畫面
	device->Present( NULL, NULL, NULL, NULL );
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 設定鏡頭轉換矩陣
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float *) &view_matrix);
	// 計算出一個非平行投影的透視矩陣
	Matrix4x4 perspective_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float *) &perspective_matrix);

	// 畫出金字塔的8條邊線
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	glDrawArrays(GL_LINES, 0, 16);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
