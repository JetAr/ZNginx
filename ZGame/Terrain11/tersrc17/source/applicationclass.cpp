////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
    m_Input = 0;
    m_Direct3D = 0;
    m_Timer = 0;
    m_Position = 0;
    m_Camera = 0;
    m_Light = 0;
    m_Terrain = 0;
    m_TerrainShader = 0;
    m_ColorTexture1 = 0;
    m_ColorTexture2 = 0;
    m_ColorTexture3 = 0;
    m_ColorTexture4 = 0;
    m_AlphaTexture1 = 0;
    m_NormalTexture1 = 0;
    m_NormalTexture2 = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
    bool result;


    // Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
    m_Input = new InputClass;
    if(!m_Input)
    {
        return false;
    }

    // Initialize the input object.
    result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
        return false;
    }

    // Create the Direct3D object.
    m_Direct3D = new D3DClass;
    if(!m_Direct3D)
    {
        return false;
    }

    // Initialize the Direct3D object.
    result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
        return false;
    }

    // Create the timer object.
    m_Timer = new TimerClass;
    if(!m_Timer)
    {
        return false;
    }

    // Initialize the timer object.
    result = m_Timer->Initialize();
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
        return false;
    }

    // Create the position object.
    m_Position = new PositionClass;
    if(!m_Position)
    {
        return false;
    }

    // Set the initial position.
    m_Position->SetPosition(14.0f, 13.0f, 10.0f);
    m_Position->SetRotation(25.0f, 0.0f, 0.0f);

    // Create the camera object.
    m_Camera = new CameraClass;
    if(!m_Camera)
    {
        return false;
    }

    // Create the light object.
    m_Light = new LightClass;
    if(!m_Light)
    {
        return false;
    }

    // Initialize the light object.
    m_Light->SetDirection(0.5f, -0.75f, 0.0f);

    // Create the terrain object.
    m_Terrain = new TerrainClass;
    if(!m_Terrain)
    {
        return false;
    }

    // Initialize the terrain object.
    result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../Engine/data/hm.bmp", 10.0f);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
        return false;
    }

    // Create the terrain shader object.
    m_TerrainShader = new TerrainShaderClass;
    if(!m_TerrainShader)
    {
        return false;
    }

    // Initialize the terrain shader object.
    result = m_TerrainShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the first color texture object.
    m_ColorTexture1 = new TextureClass;
    if(!m_ColorTexture1)
    {
        return false;
    }

    // Load the first color texture object.
    result = m_ColorTexture1->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/dirt001.dds");
    if(!result)
    {
        return false;
    }

    // Create the second color texture object.
    m_ColorTexture2 = new TextureClass;
    if(!m_ColorTexture2)
    {
        return false;
    }

    // Load the second color texture object.
    result = m_ColorTexture2->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/dirt004.dds");
    if(!result)
    {
        return false;
    }

    // Create the third color texture object.
    m_ColorTexture3 = new TextureClass;
    if(!m_ColorTexture3)
    {
        return false;
    }

    // Load the third color texture object.
    result = m_ColorTexture3->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/dirt002.dds");
    if(!result)
    {
        return false;
    }

    // Create the fourth color texture object.
    m_ColorTexture4 = new TextureClass;
    if(!m_ColorTexture4)
    {
        return false;
    }

    // Load the fourth color texture object.
    result = m_ColorTexture4->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/stone001.dds");
    if(!result)
    {
        return false;
    }

    // Create the first alpha texture object.
    m_AlphaTexture1 = new TextureClass;
    if(!m_AlphaTexture1)
    {
        return false;
    }

    // Load the first alpha texture object.
    result = m_AlphaTexture1->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/alpha001.dds");
    if(!result)
    {
        return false;
    }

    // Create the first normal texture object.
    m_NormalTexture1 = new TextureClass;
    if(!m_NormalTexture1)
    {
        return false;
    }

    // Load the first alpha texture object.
    result = m_NormalTexture1->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/normal001.dds");
    if(!result)
    {
        return false;
    }

    // Create the second normal texture object.
    m_NormalTexture2 = new TextureClass;
    if(!m_NormalTexture2)
    {
        return false;
    }

    // Load the second alpha texture object.
    result = m_NormalTexture2->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/normal002.dds");
    if(!result)
    {
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the texture objects.
    if(m_ColorTexture1)
    {
        m_ColorTexture1->Shutdown();
        delete m_ColorTexture1;
        m_ColorTexture1 = 0;
    }

    if(m_ColorTexture2)
    {
        m_ColorTexture2->Shutdown();
        delete m_ColorTexture2;
        m_ColorTexture2 = 0;
    }

    if(m_ColorTexture3)
    {
        m_ColorTexture3->Shutdown();
        delete m_ColorTexture3;
        m_ColorTexture3 = 0;
    }

    if(m_ColorTexture4)
    {
        m_ColorTexture4->Shutdown();
        delete m_ColorTexture4;
        m_ColorTexture4 = 0;
    }

    if(m_AlphaTexture1)
    {
        m_AlphaTexture1->Shutdown();
        delete m_AlphaTexture1;
        m_AlphaTexture1 = 0;
    }

    if(m_NormalTexture1)
    {
        m_NormalTexture1->Shutdown();
        delete m_NormalTexture1;
        m_NormalTexture1 = 0;
    }

    if(m_NormalTexture2)
    {
        m_NormalTexture2->Shutdown();
        delete m_NormalTexture2;
        m_NormalTexture2 = 0;
    }

    // Release the terrain shader object.
    if(m_TerrainShader)
    {
        m_TerrainShader->Shutdown();
        delete m_TerrainShader;
        m_TerrainShader = 0;
    }

    // Release the terrain object.
    if(m_Terrain)
    {
        m_Terrain->Shutdown();
        delete m_Terrain;
        m_Terrain = 0;
    }

    // Release the light object.
    if(m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // Release the camera object.
    if(m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // Release the position object.
    if(m_Position)
    {
        delete m_Position;
        m_Position = 0;
    }

    // Release the timer object.
    if(m_Timer)
    {
        delete m_Timer;
        m_Timer = 0;
    }

    // Release the Direct3D object.
    if(m_Direct3D)
    {
        m_Direct3D->Shutdown();
        delete m_Direct3D;
        m_Direct3D = 0;
    }

    // Release the input object.
    if(m_Input)
    {
        m_Input->Shutdown();
        delete m_Input;
        m_Input = 0;
    }

    return;
}


bool ApplicationClass::Frame()
{
    bool result;


    // Update the system stats.
    m_Timer->Frame();

    // Read the user input.
    result = m_Input->Frame();
    if(!result)
    {
        return false;
    }

    // Check if the user pressed escape and wants to exit the application.
    if(m_Input->IsEscapePressed() == true)
    {
        return false;
    }

    // Do the frame input processing.
    result = HandleMovementInput(m_Timer->GetTime());
    if(!result)
    {
        return false;
    }

    // Render the graphics.
    result = Render();
    if(!result)
    {
        return false;
    }

    return result;
}


bool ApplicationClass::HandleMovementInput(float frameTime)
{
    bool keyDown;
    float posX, posY, posZ, rotX, rotY, rotZ;


    // Set the frame time for calculating the updated position.
    m_Position->SetFrameTime(frameTime);

    // Handle the input.
    keyDown = m_Input->IsLeftPressed();
    m_Position->TurnLeft(keyDown);

    keyDown = m_Input->IsRightPressed();
    m_Position->TurnRight(keyDown);

    keyDown = m_Input->IsUpPressed();
    m_Position->MoveForward(keyDown);

    keyDown = m_Input->IsDownPressed();
    m_Position->MoveBackward(keyDown);

    keyDown = m_Input->IsAPressed();
    m_Position->MoveUpward(keyDown);

    keyDown = m_Input->IsZPressed();
    m_Position->MoveDownward(keyDown);

    keyDown = m_Input->IsPgUpPressed();
    m_Position->LookUpward(keyDown);

    keyDown = m_Input->IsPgDownPressed();
    m_Position->LookDownward(keyDown);

    // Get the view point position/rotation.
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);

    // Set the position of the camera.
    m_Camera->SetPosition(posX, posY, posZ);
    m_Camera->SetRotation(rotX, rotY, rotZ);

    return true;
}


bool ApplicationClass::Render()
{
    D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
    bool result;


    // Clear the scene.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, projection, ortho, and base view matrices from the camera and Direct3D objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);

    // Render the terrain using the terrain shader.
    m_Terrain->Render(m_Direct3D->GetDeviceContext());
    result = m_TerrainShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetDirection(), m_ColorTexture1->GetTexture(),
                                     m_ColorTexture2->GetTexture(), m_ColorTexture3->GetTexture(), m_ColorTexture4->GetTexture(), m_AlphaTexture1->GetTexture(), m_NormalTexture1->GetTexture(), m_NormalTexture2->GetTexture());
    if(!result)
    {
        return false;
    }

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}
