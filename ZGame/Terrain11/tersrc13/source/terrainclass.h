////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


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


/////////////
// GLOBALS //
/////////////
const int TEXTURE_REPEAT = 16;


////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR4 texture;
        D3DXVECTOR3 normal;
        D3DXVECTOR4 color;
    };

    struct HeightMapType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float r, g, b;
    };

    struct VectorType
    {
        float x, y, z;
    };

public:
    TerrainClass();
    TerrainClass(const TerrainClass&);
    ~TerrainClass();

    bool Initialize(ID3D11Device*, char*, WCHAR*, char*, WCHAR*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();
    ID3D11ShaderResourceView* GetTexture();
    ID3D11ShaderResourceView* GetDetailMapTexture();

private:
    bool LoadHeightMap(char*);
    void NormalizeHeightMap();
    bool CalculateNormals();
    void ShutdownHeightMap();

    void CalculateTextureCoordinates();
    bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
    void ReleaseTextures();

    bool LoadColorMap(char*);

    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    int m_terrainWidth, m_terrainHeight;
    int m_vertexCount, m_indexCount;
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    HeightMapType* m_heightMap;
    TextureClass *m_Texture, *m_DetailTexture;
};

#endif
