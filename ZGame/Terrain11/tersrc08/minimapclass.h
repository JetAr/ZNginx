////////////////////////////////////////////////////////////////////////////////
// Filename: minimapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MINIMAPCLASS_H_
#define _MINIMAPCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "bitmapclass.h"
#include "textureshaderclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: MiniMapClass
////////////////////////////////////////////////////////////////////////////////
class MiniMapClass
{
public:
    MiniMapClass();
    MiniMapClass(const MiniMapClass&);
    ~MiniMapClass();

    bool Initialize(ID3D11Device*, HWND, int, int, D3DXMATRIX, float, float);
    void Shutdown();
    bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, TextureShaderClass*);

    void PositionUpdate(float, float);

private:
    int m_mapLocationX, m_mapLocationY, m_pointLocationX, m_pointLocationY;
    float m_mapSizeX, m_mapSizeY, m_terrainWidth, m_terrainHeight;
    D3DXMATRIX m_viewMatrix;
    BitmapClass *m_MiniMapBitmap, *m_Border, *m_Point;
};

#endif
