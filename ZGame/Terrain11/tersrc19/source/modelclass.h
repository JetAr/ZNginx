﻿////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
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

    bool Initialize(ID3D11Device*, char*, WCHAR*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();
    ID3D11ShaderResourceView* GetColorTexture();

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

    bool LoadModel(char*);
    void ReleaseModel();

private:
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
    ModelType* m_model;
    TextureClass* m_ColorTexture;
};

#endif
