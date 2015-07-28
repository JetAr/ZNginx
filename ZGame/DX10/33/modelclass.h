////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <fstream>
using namespace std;


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

    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

public:
    ModelClass();
    ModelClass(const ModelClass&);
    ~ModelClass();

    bool Initialize(ID3D10Device*, char*, WCHAR*, WCHAR*, WCHAR*);
    void Shutdown();
    void Render(ID3D10Device*);

    int GetIndexCount();

    ID3D10ShaderResourceView* GetTexture1();
    ID3D10ShaderResourceView* GetTexture2();
    ID3D10ShaderResourceView* GetTexture3();

private:
    bool InitializeBuffers(ID3D10Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D10Device*);

    bool LoadTextures(ID3D10Device*, WCHAR*, WCHAR*, WCHAR*);
    void ReleaseTextures();

    bool LoadModel(char*);
    void ReleaseModel();

private:
    ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
    TextureClass *m_Texture1, *m_Texture2, *m_Texture3;
    ModelType* m_model;
};

#endif
