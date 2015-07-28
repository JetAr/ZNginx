////////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTCLASS_H_
#define _FONTCLASS_H_


/////////////
// GLOBALS //
/////////////
const int SMALL_FONT = 1;
const int MEDIUM_FONT = 2;
const int LARGE_FONT = 3;


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: FontClass
////////////////////////////////////////////////////////////////////////////////
class FontClass
{
private:
    struct FontType
    {
        float left, right;
        int size;
    };

    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
    };

public:
    FontClass();
    FontClass(const FontClass&);
    ~FontClass();

    bool Initialize(ID3D11Device*, char*, WCHAR*, float, int);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();
    void BuildVertexArray(void*, char*, float, float);
    int GetSentencePixelLength(char*);
    int GetFontHeight();

private:
    bool LoadFontData(char*);
    void ReleaseFontData();
    bool LoadTexture(ID3D11Device*, WCHAR*);
    void ReleaseTexture();

private:
    FontType* m_Font;
    TextureClass* m_Texture;
    float m_fontHeight;
    int m_spaceSize;
};

#endif
