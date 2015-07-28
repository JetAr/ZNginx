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
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "shadermanagerclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "cameraclass.h"
#include "fpsclass.h"
#include "userinterfaceclass.h"
#include "modelclass.h"
#include "foliageclass.h"


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
    bool RenderSceneToTexture();

private:
    InputClass* m_Input;
    D3DClass* m_Direct3D;
    ShaderManagerClass* m_ShaderManager;
    TimerClass* m_Timer;
    PositionClass* m_Position;
    CameraClass* m_Camera;
    FpsClass* m_Fps;
    UserInterfaceClass* m_UserInterface;
    ModelClass* m_GroundModel;
    FoliageClass* m_Foliage;
};

#endif
