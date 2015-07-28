////////////////////////////////////////////////////////////////////////////////
// Filename: rendertextureclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTEXTURECLASS_H_
#define _RENDERTEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: RenderTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderTextureClass
{
public:
    RenderTextureClass();
    RenderTextureClass(const RenderTextureClass&);
    ~RenderTextureClass();

    bool Initialize(ID3D10Device*, int, int, float, float);
    void Shutdown();

    void SetRenderTarget(ID3D10Device*);
    void ClearRenderTarget(ID3D10Device*, float, float, float, float);

    ID3D10ShaderResourceView* GetShaderResourceView();

    void GetProjectionMatrix(D3DXMATRIX&);
    void GetOrthoMatrix(D3DXMATRIX&);

    int GetTextureWidth();
    int GetTextureHeight();

private:
    int m_textureWidth, m_textureHeight;
    ID3D10Texture2D* m_renderTargetTexture;
    ID3D10RenderTargetView* m_renderTargetView;
    ID3D10ShaderResourceView* m_shaderResourceView;
    ID3D10Texture2D* m_depthStencilBuffer;
    ID3D10DepthStencilView* m_depthStencilView;
    D3D10_VIEWPORT m_viewport;
    D3DXMATRIX m_projectionMatrix;
    D3DXMATRIX m_orthoMatrix;
};

#endif
