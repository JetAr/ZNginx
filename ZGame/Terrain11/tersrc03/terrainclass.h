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


////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR3 normal;
    };

    struct HeightMapType
    {
        float x, y, z;
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

    bool Initialize(ID3D11Device*, char*);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();

private:
    bool LoadHeightMap(char*);
    void NormalizeHeightMap();
    bool CalculateNormals();
    void ShutdownHeightMap();

    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

private:
    int m_terrainWidth, m_terrainHeight;
    int m_vertexCount, m_indexCount;
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    HeightMapType* m_heightMap;
};

#endif
