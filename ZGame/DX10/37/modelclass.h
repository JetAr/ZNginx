////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

    struct InstanceType
    {
        D3DXVECTOR3 position;
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D10Device*, WCHAR*);
    void Shutdown();
    void Render(ID3D10Device*);

    int GetVertexCount();
    int GetInstanceCount();
    ID3D10ShaderResourceView* GetTexture();

private:
    bool InitializeBuffers(ID3D10Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D10Device*);

    bool LoadTexture(ID3D10Device*, WCHAR*);
    void ReleaseTexture();

private:
    ID3D10Buffer* m_vertexBuffer;
    ID3D10Buffer* m_instanceBuffer;
    int m_vertexCount;
    int m_instanceCount;
    TextureClass* m_Texture;
};

#endif
