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
#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "terrainshaderclass.h"


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
        D3DXVECTOR4 color;
    };

    struct HeightMapType
    {
        float x, y, z;
        float nx, ny, nz;
        float r, g, b;
        int rIndex, gIndex, bIndex;
    };

    struct VectorType
    {
        float x, y, z;
    };

    struct MaterialGroupType
    {
        int textureIndex1, textureIndex2, alphaIndex;
        int red, green, blue;
        ID3D11Buffer *vertexBuffer, *indexBuffer;
        int vertexCount, indexCount;
        VertexType* vertices;
        unsigned long* indices;
    };

public:
    TerrainClass();
    TerrainClass(const TerrainClass&);
    ~TerrainClass();

    bool Initialize(ID3D11Device*, char*, char*, char*, char*);
    void Shutdown();

    bool Render(ID3D11DeviceContext*, TerrainShaderClass*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3);

private:
    bool LoadHeightMap(char*);
    void NormalizeHeightMap();
    bool CalculateNormals();
    void ShutdownHeightMap();

    bool LoadColorMap(char*);

    bool LoadMaterialFile(char*, char*, ID3D11Device*);
    bool LoadMaterialMap(char*);
    bool LoadMaterialBuffers(ID3D11Device*);
    void ReleaseMaterials();

private:
    int m_terrainWidth, m_terrainHeight;
    HeightMapType* m_heightMap;
    int m_textureCount, m_materialCount;
    TextureClass* m_Textures;
    MaterialGroupType* m_Materials;
};

#endif
