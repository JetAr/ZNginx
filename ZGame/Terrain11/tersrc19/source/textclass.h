////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTCLASS_H_
#define _TEXTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "fontclass.h"
#include "shadermanagerclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    TextClass();
    TextClass(const TextClass&);
    ~TextClass();

    bool Initialize(ID3D11Device*, FontClass*, int, int, int, char*, int, int, float, float, float, bool, ID3D11DeviceContext*);
    void Shutdown();
    void Render(ID3D11DeviceContext*, ShaderManagerClass*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

    bool UpdateSentence(FontClass*, char*, int, int, float, float, float, ID3D11DeviceContext*);

private:
    bool InitializeSentence(ID3D11Device*, FontClass*, char*, int, int, float, float, float, ID3D11DeviceContext*);
    void RenderSentence(ID3D11DeviceContext*, ShaderManagerClass*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

private:
    ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
    ID3D11Buffer *m_vertexBuffer2, *m_indexBuffer2;
    int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount;
    D3DXVECTOR4 m_pixelColor;
    bool m_shadow;
};

#endif
