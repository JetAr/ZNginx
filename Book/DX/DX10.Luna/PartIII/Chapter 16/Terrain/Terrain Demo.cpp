﻿//=============================================================================
// Terrain Demo.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Uses the Terrain class to render a terrain.
//
// Controls:
//		'A'/'D'/'W'/'S' - Move
//      Hold down left mouse button and move mouse to rotate.
//
//=============================================================================

#include "d3dApp.h"
#include "Light.h"
#include "Camera.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "TextureMgr.h"
#include "Sky.h"
#include "DrawableTex2D.h"
#include "Mesh.h"
#include "Terrain.h"

class TerrainApp : public D3DApp
{
public:
    TerrainApp(HINSTANCE hInstance);
    ~TerrainApp();

    void initApp();
    void onResize();
    void updateScene(float dt);
    void drawScene();

    LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

private:

    POINT mOldMousePos;

    Terrain mLand;
    Sky mSky;

    Light mParallelLight;

    D3DXMATRIX mLandWorld;

    ID3D10ShaderResourceView* mEnvMapRV;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
                   PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


    TerrainApp theApp(hInstance);

    theApp.initApp();

    return theApp.run();
}

TerrainApp::TerrainApp(HINSTANCE hInstance)
    : D3DApp(hInstance), mEnvMapRV(0)
{
    D3DXMatrixIdentity(&mLandWorld);
}

TerrainApp::~TerrainApp()
{
    if( md3dDevice )
        md3dDevice->ClearState();

    fx::DestroyAll();
    InputLayout::DestroyAll();
}

void TerrainApp::initApp()
{
    D3DApp::initApp();

    fx::InitAll(md3dDevice);
    InputLayout::InitAll(md3dDevice);
    GetTextureMgr().init(md3dDevice);

    mClearColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    GetCamera().position() = D3DXVECTOR3(0.0f, 10.0f, -10.0f);

    mEnvMapRV = GetTextureMgr().createCubeTex(L"grassenvmap1024.dds");
    mSky.init(md3dDevice, mEnvMapRV, 5000.0f);

    Terrain::InitInfo tii;
    tii.HeightmapFilename = L"coastMountain513.raw";
    tii.LayerMapFilename0 = L"grass.dds";
    tii.LayerMapFilename1 = L"lightdirt.dds";
    tii.LayerMapFilename2 = L"darkdirt.dds";
    tii.LayerMapFilename3 = L"stone.dds";
    tii.LayerMapFilename4 = L"snow.dds";
    tii.BlendMapFilename  = L"blend.dds";
    tii.HeightScale  = 0.35f;
    tii.HeightOffset = -20.0f;
    tii.NumRows      = 513;
    tii.NumCols      = 513;
    tii.CellSpacing  = 1.0f;

    mLand.init(md3dDevice, tii);

    mParallelLight.dir      = D3DXVECTOR3(0.707f, -0.707f, 0.0f);
    mParallelLight.ambient  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
    mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    mParallelLight.specular = D3DXCOLOR(0.5f, 0.4843f, 0.3f, 1.0f);

    mLand.setDirectionToSun(-mParallelLight.dir);
}

void TerrainApp::onResize()
{
    D3DApp::onResize();

    float aspect = (float)mClientWidth/mClientHeight;
    GetCamera().setLens(0.25f*PI, aspect, 0.5f, 1000.0f);
}

void TerrainApp::updateScene(float dt)
{
    D3DApp::updateScene(dt);

    if(GetAsyncKeyState('A') & 0x8000)	GetCamera().strafe(-40.0f*dt);
    if(GetAsyncKeyState('D') & 0x8000)	GetCamera().strafe(+40.0f*dt);
    if(GetAsyncKeyState('W') & 0x8000)	GetCamera().walk(+40.0f*dt);
    if(GetAsyncKeyState('S') & 0x8000)	GetCamera().walk(-40.0f*dt);

    GetCamera().rebuildView();
}

void TerrainApp::drawScene()
{
    D3DApp::drawScene();

    // Restore default states, input layout and primitive topology
    // because mFont->DrawText changes them.  Note that we can
    // restore the default states by passing null.
    md3dDevice->OMSetDepthStencilState(0, 0);
    float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);

    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mLand.draw(mLandWorld);

    // Draw sky last to save fill rate.
    mSky.draw();

    // We specify DT_NOCLIP, so we do not care about width/height of the rect.
    RECT R = {5, 5, 0, 0};
    md3dDevice->RSSetState(0);
    mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, WHITE);

    mSwapChain->Present(0, 0);
}

LRESULT TerrainApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    POINT mousePos;
    int dx = 0;
    int dy = 0;
    switch(msg)
    {
    case WM_LBUTTONDOWN:
        if( wParam & MK_LBUTTON )
        {
            SetCapture(mhMainWnd);

            mOldMousePos.x = LOWORD(lParam);
            mOldMousePos.y = HIWORD(lParam);
        }
        return 0;

    case WM_LBUTTONUP:
        ReleaseCapture();
        return 0;

    case WM_MOUSEMOVE:
        if( wParam & MK_LBUTTON )
        {
            mousePos.x = (int)LOWORD(lParam);
            mousePos.y = (int)HIWORD(lParam);

            dx = mousePos.x - mOldMousePos.x;
            dy = mousePos.y - mOldMousePos.y;

            GetCamera().pitch( dy * 0.0087266f );
            GetCamera().rotateY( dx * 0.0087266f );

            mOldMousePos = mousePos;
        }
        return 0;
    }

    return D3DApp::msgProc(msg, wParam, lParam);
}

