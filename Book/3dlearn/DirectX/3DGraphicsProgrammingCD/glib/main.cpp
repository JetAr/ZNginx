#include <stdio.h>
#include "Gut.h"

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

// 一個3D座標點(x,y,z)的位置
float vertex[] = {0.0f, 0.0f, 0.0f};

// 使用DirectX 9的函式
void RenderFrameDX9(void)
{
	// 取得D3D裝置.
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 關閉打光的計算
	device->SetRenderState( D3DRS_LIGHTING, FALSE );
	// 把畫面清為黑色
	device->Clear( 
		0, NULL,　// 清除整個畫面 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // 清除顏色跟Z buffer 
		D3DCOLOR_ARGB(0, 0, 0, 0),  // 設定要把顏色清成黑色
		1.0f, // 設定要把Z值清為1, 也就是離鏡頭最遠
		0 // 設定要把Stencil buffer清為0, 在這沒差.
		);

	// 開始下繪圖指令
	device->BeginScene(); 
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZ); 
	// 畫出一個點
	device->DrawPrimitiveUP(D3DPT_POINTLIST, 1, vertex, 12); 
	// 宣告所有的繪圖指令都下完了
	device->EndScene();		

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}

// 使用OpenGL的函式
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 畫一個點
	glBegin(GL_POINTS);
	glVertex3fv(vertex);
	glEnd();

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
