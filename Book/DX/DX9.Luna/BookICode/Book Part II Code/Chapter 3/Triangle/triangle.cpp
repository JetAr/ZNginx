//////////////////////////////////////////////////////////////////////////////////////////////////
//
// File: triangle.cpp
//
// Author: Frank Luna (C) All Rights Reserved
//
// System: AMD Athlon 1800+ XP, 512 DDR, Geforce 3, Windows XP, MSVC++ 7.0
//
// Desc: Renders a triangle in wireframe mode.  Demonstrates vertex buffers,
//       render states, and drawing commands.
//
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"

//
// Globals
//

IDirect3DDevice9* Device = 0;

const int Width  = 640;
const int Height = 480;

IDirect3DVertexBuffer9* Triangle = 0; // vertex buffer to store
// our triangle data.

//
// Classes and Structures
//

struct Vertex
{
    Vertex() {}

    Vertex(float x, float y, float z)
    {
        _x = x;
        _y = y;
        _z = z;
    }

    float _x, _y, _z;

    static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;

//
// Framework Functions
//
bool Setup()
{
    //
    // Create the vertex buffer.
    //

    Device->CreateVertexBuffer(
        3 * sizeof(Vertex), // size in bytes
        D3DUSAGE_WRITEONLY, // flags
        Vertex::FVF,        // vertex format
        D3DPOOL_MANAGED,    // managed memory pool
        &Triangle,          // return create vertex buffer
        0);                 // not used - set to 0

    //
    // Fill the buffers with the triangle data.
    //

    Vertex* vertices;
    Triangle->Lock(0, 0, (void**)&vertices, 0);

    vertices[0] = Vertex(-1.0f, 0.0f, 2.0f);
    vertices[1] = Vertex( 0.0f, 1.0f, 2.0f);
    vertices[2] = Vertex( 1.0f, 0.0f, 2.0f);

    Triangle->Unlock();

    //
    // Set the projection matrix.
    //
    //z 2015-10-20 09:49 设置 projection matrix
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(
        &proj,                        // result
        D3DX_PI * 0.5f,               // 90 - degrees
        (float)Width / (float)Height, // aspect ratio
        1.0f,                         // near plane
        1000.0f);                     // far plane
    Device->SetTransform(D3DTS_PROJECTION, &proj);

    //
    // Set wireframe mode render state.
    //
    //z 2015-10-20 09:50 设置填充模式
    Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    return true;
}
void Cleanup()
{
    d3d::Release<IDirect3DVertexBuffer9*>(Triangle);
}

bool Display(float timeDelta)//z 时间间隔，目前未用到
{
    if( Device )
    {
        //z 2015-10-20 09:50 属于 clear ，很多地方的封装是单独放置的
        Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
        Device->BeginScene();
        
        //z 2015-10-20 09:50 stream source
        Device->SetStreamSource(0, Triangle, 0, sizeof(Vertex));
        //z 顶点数据格式
        Device->SetFVF(Vertex::FVF);

        // Draw one triangle.
        //z 开始绘制
        Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

        Device->EndScene();
        //z 呈现
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
        //z 2015-10-20 09:51 关闭程序
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
