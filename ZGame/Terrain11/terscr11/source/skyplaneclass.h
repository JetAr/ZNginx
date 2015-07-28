////////////////////////////////////////////////////////////////////////////////
// Filename: skyplaneclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKYPLANECLASS_H_
#define _SKYPLANECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SkyPlaneClass
////////////////////////////////////////////////////////////////////////////////
class SkyPlaneClass
{
private:
    struct SkyPlaneType
    {
        float x, y, z;
        float tu, tv;
    };

    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    SkyPlaneClass();
    SkyPlaneClass(const SkyPlaneClass&);
    ~SkyPlaneClass();

    bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    void Frame();

    int GetIndexCount();
    ID3D11ShaderResourceView* GetCloudTexture1();
    ID3D11ShaderResourceView* GetCloudTexture2();

    float GetBrightness();
    float GetTranslation(int);

private:
    bool InitializeSkyPlane(int, float, float, float, int);
    void ShutdownSkyPlane();

    bool InitializeBuffers(ID3D11Device*, int);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
    void ReleaseTextures();

private:
    SkyPlaneType* m_skyPlane;
    int m_vertexCount, m_indexCount;
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    TextureClass *m_CloudTexture1, *m_CloudTexture2;
    float m_brightness;
    float m_translationSpeed[4];
    float m_textureTranslation[4];
};

#endif
