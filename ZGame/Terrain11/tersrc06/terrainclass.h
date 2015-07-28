////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


/////////////
// GLOBALS //
/////////////
const int TEXTURE_REPEAT = 8;


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
        D3DXVECTOR3 normal;
    };

    struct HeightMapType
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
    TerrainClass();
    TerrainClass(const TerrainClass&);
    ~TerrainClass();

    bool Initialize(ID3D11Device*, char*, WCHAR*);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    int GetVertexCount();
    void CopyVertexArray(void*);

private:
    bool LoadHeightMap(char*);
    void NormalizeHeightMap();
    bool CalculateNormals();
    void ShutdownHeightMap();

    void CalculateTextureCoordinates();
    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();

private:
    int m_terrainWidth, m_terrainHeight;
    HeightMapType* m_heightMap;
    TextureClass* m_Texture;
    int m_vertexCount;
    VertexType* m_vertices;
};

#endif
