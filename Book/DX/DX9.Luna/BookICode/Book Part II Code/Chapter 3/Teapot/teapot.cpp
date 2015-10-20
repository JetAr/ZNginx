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

IDirect3DDevice9* Device = 0;

//z 窗口大小
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
    //z 创建 Teapot 
    D3DXCreateTeapot(Device, &Teapot, 0);

    //
    // Position and aim the camera.
    //
    
    D3DXVECTOR3 position(0.0f, 0.0f, -3.0f);
    D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
    D3DXMATRIX V;
    //z 初始化和设置 view matrix
    D3DXMatrixLookAtLH(&V, &position, &target, &up);
    Device->SetTransform(D3DTS_VIEW, &V);

    //
    // Set projection matrix.
    //
    //z 2015-10-20 09:54 设置和初始化 D3DTS_PROJECTION 
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,
        D3DX_PI * 0.5f, // 90 - degree
        (float)Width / (float)Height,
        1.0f,
        1000.0f);
    Device->SetTransform(D3DTS_PROJECTION, &proj);

    //
    // Switch to wireframe mode.
    //
    //z 设置填充方式
    Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    return true;
}
void Cleanup()
{
    d3d::Release<ID3DXMesh*>(Teapot);
}

bool Display(float timeDelta)
{
    if( Device )
    {
        //
        // Spin the teapot:
        //

        D3DXMATRIX Ry;
        static float y = 0.0f;
        D3DXMatrixRotationY(&Ry, y);
        
        y += timeDelta;
        if(y >= 6.28f)//z 2*PI，一圈的样子
            y = 0.0f;

        //z 2015-10-20 09:57
        //z 变换世界坐标系，每次都重新设置世界坐标系。实际的App中是如何进行设置的了？
        Device->SetTransform(D3DTS_WORLD, &Ry);

        //
        // Draw the Scene:
        //
        Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
        Device->BeginScene();

        // Draw teapot using DrawSubset method with 0 as the argument.
        Teapot->DrawSubset(0);

        Device->EndScene();
        Device->Present(0, 0, 0, 0);
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
                     Width, Height, true, D3DDEVTYPE_HAL, &Device))
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

    Device->Release();

    return 0;
}
