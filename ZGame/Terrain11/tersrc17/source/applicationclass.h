////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _APPLICATIONCLASS_H_
#define _APPLICATIONCLASS_H_


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "cameraclass.h"
#include "lightclass.h"
#include "terrainclass.h"
#include "terrainshaderclass.h"
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ApplicationClass
////////////////////////////////////////////////////////////////////////////////
class ApplicationClass
{
public:
    ApplicationClass();
    ApplicationClass(const ApplicationClass&);
    ~ApplicationClass();

    bool Initialize(HINSTANCE, HWND, int, int);
    void Shutdown();
    bool Frame();

private:
    bool HandleMovementInput(float);
    bool Render();

private:
    InputClass* m_Input;
    D3DClass* m_Direct3D;
    TimerClass* m_Timer;
    PositionClass* m_Position;
    CameraClass* m_Camera;
    LightClass* m_Light;
    TerrainClass* m_Terrain;
    TerrainShaderClass* m_TerrainShader;

    TextureClass *m_ColorTexture1;
    TextureClass *m_ColorTexture2;
    TextureClass *m_ColorTexture3;
    TextureClass *m_ColorTexture4;
    TextureClass *m_AlphaTexture1;
    TextureClass *m_NormalTexture1;
    TextureClass *m_NormalTexture2;
};

#endif
