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
Vector4 g_eye(0.0f,0.0f,5.0f); 
// 鏡頭對準的點
Vector4 g_lookat(0.0f, 0.0f, 0.0f); 
// 鏡頭正上方的方向
Vector4 g_up(0.0f, 1.0f,0.0f); 

// 使用DirectX9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
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
	// 計算出一個使用平行投影的矩陣
	Matrix4x4 orthogonal_matrix = GutMatrixOrthoRH_DirectX(5.0f, 5.0f, 0.1f, 100.0f);
	// 把這兩個矩陣相乘
	Matrix4x4 view_orthogonal_matrix = view_matrix * orthogonal_matrix;
	// 把空間中的座標點轉換到螢幕座標系上
	Vector4 vertices[16];
	for ( int i=0; i<16; i++ )
	{
		vertices[i] = g_vertices[i] * view_orthogonal_matrix;
		vertices[i] /= vertices[i].GetW();
	}

	device->BeginScene(); // 開始下繪圖指令
	device->SetFVF(D3DFVF_XYZ); // 設定資料格式
	// 畫出金字塔的8條邊線
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, vertices, sizeof(Vector4)); 
	device->EndScene(); // 宣告所有的繪圖指令都下完了

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 計算出一個使用平行投影的矩陣
	Matrix4x4 orthogonal_matrix = GutMatrixOrthoRH_OpenGL(5.0f, 5.0f, 0.1f, 100.0f);
	// 把這兩個矩陣相乘
	Matrix4x4 view_orthogonal_matrix = view_matrix * orthogonal_matrix;
	// 把空間中的座標點轉換到螢幕座標系上
	Vector4 vertices[16];
	for ( int i=0; i<16; i++ )
	{
		vertices[i] = g_vertices[i] * view_orthogonal_matrix;
		vertices[i] /= vertices[i].GetW();
	}

	// 畫出金字塔的8條邊線
	glBegin(GL_LINES);
	for ( int l=0; l<8; l++ )
	{
		glVertex3fv((float *) &vertices[l*2]);
		glVertex3fv((float *) &vertices[l*2+1]);
	}
	glEnd();

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
