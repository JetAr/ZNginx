////////////////////////////////////////////////////////////////////////////////
// Filename: orthowindowclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ORTHOWINDOWCLASS_H_
#define _ORTHOWINDOWCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: OrthoWindowClass
////////////////////////////////////////////////////////////////////////////////
class OrthoWindowClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    OrthoWindowClass();
    OrthoWindowClass(const OrthoWindowClass&);
    ~OrthoWindowClass();

    bool Initialize(ID3D10Device*, int, int);
    void Shutdown();
    void Render(ID3D10Device*);

    int GetIndexCount();

private:
    bool InitializeBuffers(ID3D10Device*, int, int);
    void ShutdownBuffers();
    void RenderBuffers(ID3D10Device*);

private:
    ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
    int m_vertexCount, m_indexCount;
};

#endif
