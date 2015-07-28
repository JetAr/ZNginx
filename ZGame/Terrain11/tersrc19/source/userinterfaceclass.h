////////////////////////////////////////////////////////////////////////////////
// Filename: userinterfaceclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _USERINTERFACECLASS_H_
#define _USERINTERFACECLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: UserInterfaceClass
////////////////////////////////////////////////////////////////////////////////
class UserInterfaceClass
{
public:
    UserInterfaceClass();
    UserInterfaceClass(const UserInterfaceClass&);
    ~UserInterfaceClass();

    bool Initialize(D3DClass*, int, int);
    void Shutdown();

    bool Frame(int, float, float, float, float, float, float, ID3D11DeviceContext*);
    void Render(D3DClass*, ShaderManagerClass*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

private:
    bool UpdateFpsString(int, ID3D11DeviceContext*);
    bool UpdatePositionStrings(float, float, float, float, float, float, ID3D11DeviceContext*);

private:
    FontClass* m_Font1;
    TextClass* m_FpsString;
    int m_previousFps;
    TextClass *m_PositionStrings, *m_VideoStrings;
    int m_previousPosition[6];
};

#endif
