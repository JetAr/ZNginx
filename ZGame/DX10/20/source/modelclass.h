﻿////////////////////////////////////////////////////////////////////////////////
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
        D3DXVECTOR3 normal;
        D3DXVECTOR3 tangent;
        D3DXVECTOR3 binormal;
    };

    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
    };

    struct TempVertexType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    struct VectorType
    {
        float x, y, z;
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

    void CalculateModelVectors();
    void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
    void CalculateNormal(VectorType, VectorType, VectorType&);

private:
    ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
    ModelType* m_model;
    TextureArrayClass* m_TextureArray;
};

#endif
