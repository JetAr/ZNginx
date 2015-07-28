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
    struct HeightMapType
    {
        float x, y, z;
        float nx, ny, nz;
    };

    struct VectorType
    {
        float x, y, z;
    };

    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
        float tx, ty, tz;
        float bx, by, bz;
        float tu2, tv2;
    };

    struct TempVertexType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
        D3DXVECTOR3 normal;
        D3DXVECTOR3 tangent;
        D3DXVECTOR3 binormal;
        D3DXVECTOR2 texture2;
    };

public:
    TerrainClass();
    TerrainClass(const TerrainClass&);
    ~TerrainClass();

    bool Initialize(ID3D11Device*, char*, float);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

    int GetIndexCount();

private:
    bool LoadHeightMap(char*);
    void ReduceHeightMap(float);
    bool CalculateNormals();
    bool BuildModel();
    void CalculateModelVectors();
    void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
    bool InitializeBuffers(ID3D11Device*);

    void ReleaseHeightMap();
    void ReleaseModel();
    void ReleaseBuffers();

    void RenderBuffers(ID3D11DeviceContext*);

private:
    int m_terrainWidth, m_terrainHeight, m_vertexCount, m_indexCount;
    HeightMapType* m_heightMap;
    ModelType* m_model;
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
};

#endif
