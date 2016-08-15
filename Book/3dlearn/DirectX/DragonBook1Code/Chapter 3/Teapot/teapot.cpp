//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: teapot.cpp
// 
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders a teapot in wireframe mode.  Shows how to create a teapot 
//       using the D3DXCreateTeapot function and how to render the teapot
//       using the ID3DXMesh::DrawSubset method.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* g_device = 0; 

const int Width  = 640;
const int Height = 480;

// Mesh interface that will store the teapot data and contains
// ma method to render the teapot data.
ID3DXMesh* Teapot = 0;

//
// Framework Functions
//
bool Setup()
{
	//
	// Create the teapot geometry.
	//

	D3DXCreateTeapot(g_device, &Teapot, 0);

	//
	// Position and aim the camera.
	//

	D3DXVECTOR3 position(0.0f, 0.0f, -3.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	g_device->SetTransform(D3DTS_VIEW, &V);

	//
	// Set projection matrix.
	//

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI * 0.5f, // 90 - degree
			(float)Width / (float)Height,
			1.0f,
			1000.0f);
	g_device->SetTransform(D3DTS_PROJECTION, &proj);

	//
	// Switch to wireframe mode.
	//

	g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}
void Cleanup()
{
	d3d::Release<ID3DXMesh*>(Teapot);
}

bool Display(float timeDelta)
{
	if( g_device )
	{
		//
		// Spin the teapot:
		//

		D3DXMATRIX Ry;
		static float y = 0.0f;
		D3DXMatrixRotationY(&Ry, y);

		y += timeDelta;
		if(y >= 6.28f)
			y = 0.0f;

		g_device->SetTransform(D3DTS_WORLD, &Ry);

		//
		// Draw the Scene:
		//
		g_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		g_device->BeginScene();

		// Draw teapot using DrawSubset method with 0 as the argument.
		Teapot->DrawSubset(0);

		g_device->EndScene();
		g_device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	if(!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &g_device))
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	g_device->Release();

	return 0;
}