////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: BitmapClass
////////////////////////////////////////////////////////////////////////////////
class BitmapClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    BitmapClass();
    BitmapClass(const BitmapClass&);
    ~BitmapClass();

    bool Initialize(ID3D10Device*, int, int, WCHAR*, WCHAR*, int, int);
    void Shutdown();
    bool Render(ID3D10Device*, int, int);

    int GetIndexCount();
    ID3D10ShaderResourceView* GetTexture();
    ID3D10ShaderResourceView* GetGlowMap();

private:
    bool InitializeBuffers(ID3D10Device*);
    void ShutdownBuffers();
    bool UpdateBuffers(int, int);
    void RenderBuffers(ID3D10Device*);

    bool LoadTextures(ID3D10Device*, WCHAR*, WCHAR*);
    void ReleaseTextures();

private:
    ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
    TextureClass* m_Texture;
    TextureClass* m_GlowMap;
    int m_screenWidth, m_screenHeight;
    int m_bitmapWidth, m_bitmapHeight;
    int m_previousPosX, m_previousPosY;
};

#endif
