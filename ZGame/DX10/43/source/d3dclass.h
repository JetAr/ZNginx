////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dxgi.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
    D3DClass();
    D3DClass(const D3DClass&);
    ~D3DClass();

    bool Initialize(int, int, bool, HWND, bool, float, float);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D10Device* GetDevice();

    void GetProjectionMatrix(D3DXMATRIX&);
    void GetWorldMatrix(D3DXMATRIX&);
    void GetOrthoMatrix(D3DXMATRIX&);

    void GetVideoCardInfo(char*, int&);

    void TurnZBufferOn();
    void TurnZBufferOff();

    void SetBackBufferRenderTarget();
    void ResetViewport();

private:
    bool m_vsync_enabled;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain* m_swapChain;
    ID3D10Device* m_device;
    ID3D10RenderTargetView* m_renderTargetView;
    ID3D10Texture2D* m_depthStencilBuffer;
    ID3D10DepthStencilState* m_depthStencilState;
    ID3D10DepthStencilView* m_depthStencilView;
    ID3D10RasterizerState* m_rasterState;
    D3DXMATRIX m_projectionMatrix;
    D3DXMATRIX m_worldMatrix;
    D3DXMATRIX m_orthoMatrix;
    ID3D10DepthStencilState* m_depthDisabledStencilState;
    D3D10_VIEWPORT m_viewport;
};

#endif
