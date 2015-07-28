////////////////////////////////////////////////////////////////////////////////
// Filename: treeclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TREECLASS_H_
#define _TREECLASS_H_


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
// Class name: TreeClass
////////////////////////////////////////////////////////////////////////////////
class TreeClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
        D3DXVECTOR3 normal;
    };

    struct ModelType
    {
        float x, y, z;
        float tu, tv;
        float nx, ny, nz;
    };

public:
    TreeClass();
    TreeClass(const TreeClass&);
    ~TreeClass();

    bool Initialize(ID3D10Device*, char*, WCHAR*, char*, WCHAR*, float);
    void Shutdown();

    void RenderTrunk(ID3D10Device*);
    void RenderLeaves(ID3D10Device*);

    int GetTrunkIndexCount();
    int GetLeafIndexCount();

    ID3D10ShaderResourceView* GetTrunkTexture();
    ID3D10ShaderResourceView* GetLeafTexture();

    void SetPosition(float, float, float);
    void GetPosition(float&, float&, float&);

private:
    bool InitializeTrunkBuffers(ID3D10Device*, float);
    bool InitializeLeafBuffers(ID3D10Device*, float);

    void ShutdownBuffers();

    void RenderTrunkBuffers(ID3D10Device*);
    void RenderLeafBuffers(ID3D10Device*);

    bool LoadTextures(ID3D10Device*, WCHAR*, WCHAR*);
    void ReleaseTextures();

    bool LoadModel(char*);
    void ReleaseModel();

private:
    ID3D10Buffer *m_trunkVertexBuffer, *m_trunkIndexBuffer, *m_leafVertexBuffer, *m_leafIndexBuffer;
    int m_vertexCount, m_indexCount, m_trunkIndexCount, m_leafIndexCount;
    TextureClass *m_TrunkTexture, *m_LeafTexture;
    ModelType* m_model;
    float m_positionX, m_positionY, m_positionZ;
};

#endif
