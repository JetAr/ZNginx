//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: d3dxcreate.cpp
// 
// Author: Frank D. Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0 
//
// Desc: Renders several D3DX shapes in wireframe mode and has the camera
//       fly around the scene.  Demonstrates the D3DXCreate* functions, and
//       demonstrates more complex transformations used to position the objects
//       in the world and move the camera around the world.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9*     g_device = 0; 

const int Width  = 640;
const int Height = 480;

// Store 5 objects.
ID3DXMesh* Objects[5] = {0, 0, 0, 0, 0};

// World matrices for each object.  These matrices
// specify the locations of the objects in the world.
D3DXMATRIX ObjWorldMatrices[5];

//
// Framework Functions
//
bool Setup()
{
	//
	// Create the objects.
	//

	D3DXCreateTeapot(
		g_device,
		&Objects[0],
		0);

	D3DXCreateBox(
		g_device,
		2.0f, // width
		2.0f, // height
		2.0f, // depth
		&Objects[1],
		0);

	// cylinder is built aligned on z-axis
	D3DXCreateCylinder(
		g_device,
		1.0f, // radius at negative z end
		1.0f, // radius at positive z end
		3.0f, // length of cylinder
		10,   // slices
		10,   // stacks
		&Objects[2],
		0);

	D3DXCreateTorus(
		g_device,
		1.0f, // inner radius
		3.0f, // outer radius
		10,   // sides
		10,   // rings
		&Objects[3],
		0);

	D3DXCreateSphere(
		g_device,
		1.0f, // radius
		10,   // slices
		10,   // stacks
		&Objects[4],
		0);

	//
	// Build world matrices - position the objects in world space.
	// For example, ObjWorldMatrices[1] will position Objects[1] at
	// (-5, 0, 5).  Likewise, ObjWorldMatrices[2] will position
	// Objects[2] at (5, 0, 5).
	//

	D3DXMatrixTranslation(&ObjWorldMatrices[0],  0.0f, 0.0f,  0.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[1], -5.0f, 0.0f,  5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[2],  5.0f, 0.0f,  5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[3], -5.0f, 0.0f, -5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[4],  5.0f, 0.0f, -5.0f);

	//
	// Set the projection matrix.
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
	for(int i = 0; i < 5; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

bool Display(float timeDelta)
{
	if( g_device )
	{
		// Animate the camera:
		// The camera will circle around the center of the scene.  We use the
		// sin and cos functions to generate points on the circle, then scale them
		// by 10 to further the radius.  In addition the camera will move up and down
		// as it circles about the scene.
		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float cameraHeight = 0.0f;
		static float cameraHeightDirection = 5.0f;
		
		D3DXVECTOR3 position( cosf(angle) * 10.0f, cameraHeight, sinf(angle) * 10.0f );

		// the camera is targetted at the origin of the world
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);

		// the worlds up vector
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		g_device->SetTransform(D3DTS_VIEW, &V);

		// compute the position for the next frame
		angle += timeDelta;
		if( angle >= 6.28f )
			angle = 0.0f;

		// compute the height of the camera for the next frame
		cameraHeight += cameraHeightDirection * timeDelta;
		if( cameraHeight >= 10.0f )
			cameraHeightDirection = -5.0f;

		if( cameraHeight <= -10.0f )
			cameraHeightDirection = 5.0f;

		//
		// Draw the Scene:
		//

		g_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		g_device->BeginScene();

		for(int i = 0; i < 5; i++)
		{
			// Set the world matrix that positions the object.
			g_device->SetTransform(D3DTS_WORLD, &ObjWorldMatrices[i]);

			// Draw the object using the previously set world matrix.
			Objects[i]->DrawSubset(0);
		}

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