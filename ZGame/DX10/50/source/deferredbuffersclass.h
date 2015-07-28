////////////////////////////////////////////////////////////////////////////////
// Filename: deferredbuffersclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEFERREDBUFFERSCLASS_H_
#define _DEFERREDBUFFERSCLASS_H_


/////////////
// DEFINES //
/////////////
const int BUFFER_COUNT = 2;


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: DeferredBuffersClass
////////////////////////////////////////////////////////////////////////////////
class DeferredBuffersClass
{
public:
    DeferredBuffersClass();
    DeferredBuffersClass(const DeferredBuffersClass&);
    ~DeferredBuffersClass();

    bool Initialize(ID3D10Device*, int, int, float, float);
    void Shutdown();

    void SetRenderTargets(ID3D10Device*);
    void ClearRenderTargets(ID3D10Device*, float, float, float, float);

    ID3D10ShaderResourceView* GetShaderResourceView(int);

private:
    int m_textureWidth, m_textureHeight;
    ID3D10Texture2D* m_renderTargetTextureArray[BUFFER_COUNT];
    ID3D10RenderTargetView* m_renderTargetViewArray[BUFFER_COUNT];
    ID3D10ShaderResourceView* m_shaderResourceViewArray[BUFFER_COUNT];
    ID3D10Texture2D* m_depthStencilBuffer;
    ID3D10DepthStencilView* m_depthStencilView;
    D3D10_VIEWPORT m_viewport;
};

#endif
