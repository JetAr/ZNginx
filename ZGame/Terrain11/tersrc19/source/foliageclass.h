////////////////////////////////////////////////////////////////////////////////
// Filename: foliageclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FOLIAGECLASS_H_
#define _FOLIAGECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <time.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FoliageClass
////////////////////////////////////////////////////////////////////////////////
class FoliageClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

    struct FoliageType
    {
        float x, z;
        float r, g, b;
    };

    struct InstanceType
    {
        D3DXMATRIX matrix;
        D3DXVECTOR3 color;
    };

public:
    FoliageClass();
    FoliageClass(const FoliageClass&);
    ~FoliageClass();

    bool Initialize(ID3D11Device*, WCHAR*, int);
    void Shutdown();
    void Render(ID3D11DeviceContext*);
    bool Frame(D3DXVECTOR3, ID3D11DeviceContext*);

    int GetVertexCount();
    int GetInstanceCount();

    ID3D11ShaderResourceView* GetTexture();

private:
    bool InitializeBuffers(ID3D11Device*);
    void ShutdownBuffers();
    void RenderBuffers(ID3D11DeviceContext*);

    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

    bool GeneratePositions();

private:
    int m_foliageCount;
    FoliageType* m_foliageArray;
    InstanceType* m_Instances;
    ID3D11Buffer *m_vertexBuffer, *m_instanceBuffer;
    int m_vertexCount, m_instanceCount;
    TextureClass* m_Texture;
    float m_windRotation;
    int m_windDirection;
};

#endif
