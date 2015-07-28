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
#include "texturearrayclass.h"


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

    bool Initialize(ID3D10Device*, char*, WCHAR*, WCHAR*);
    void Shutdown();
    void Render(ID3D10Device*);

    int GetIndexCount();
    ID3D10ShaderResourceView** GetTextureArray();

private:
    bool InitializeBuffers(ID3D10Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D10Device*);

    bool LoadTextures(ID3D10Device*, WCHAR*, WCHAR*);
    void ReleaseTextures();

    bool LoadModel(char*);
    void ReleaseModel();

private:
    ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
    ModelType* m_model;
    TextureArrayClass* m_TextureArray;
};

#endif
