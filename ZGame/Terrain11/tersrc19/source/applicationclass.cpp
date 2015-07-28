////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
    m_Input = 0;
    m_Direct3D = 0;
    m_ShaderManager = 0;
    m_Timer = 0;
    m_Position = 0;
    m_Camera = 0;
    m_Fps = 0;
    m_UserInterface = 0;
    m_GroundModel = 0;
    m_Foliage = 0;
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


    // Create the input object.
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

    // Create the shader manager object.
    m_ShaderManager = new ShaderManagerClass;
    if(!m_ShaderManager)
    {
        return false;
    }

    // Initialize the shader manager object.
    result = m_ShaderManager->Initialize(m_Direct3D, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
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
    m_Position->SetPosition(0.0f, 1.5f, -4.0f);
    m_Position->SetRotation(15.0f, 0.0f, 0.0f);

    // Create the camera object.
    m_Camera = new CameraClass;
    if(!m_Camera)
    {
        return false;
    }

    // Set the initial position of the camera and build the matrices needed for rendering.
    m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
    m_Camera->Render();
    m_Camera->RenderBaseViewMatrix();

    // Create the fps object.
    m_Fps = new FpsClass;
    if(!m_Fps)
    {
        return false;
    }

    // Initialize the fps object.
    m_Fps->Initialize();

    // Create the user interface object.
    m_UserInterface = new UserInterfaceClass;
    if(!m_UserInterface)
    {
        return false;
    }

    // Initialize the user interface object.
    result = m_UserInterface->Initialize(m_Direct3D, screenWidth, screenHeight);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the user interface object.", L"Error", MB_OK);
        return false;
    }

    // Create the ground model object.
    m_GroundModel = new ModelClass;
    if(!m_GroundModel)
    {
        return false;
    }

    // Initialize the ground model object.
    result = m_GroundModel->Initialize(m_Direct3D->GetDevice(), "../Engine/data/plane01.txt", L"../Engine/data/rock015.dds");
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    // Create the foliage object.
    m_Foliage = new FoliageClass;
    if(!m_Foliage)
    {
        return false;
    }

    // Initialize the foliage object.
    result = m_Foliage->Initialize(m_Direct3D->GetDevice(), L"../Engine/data/grass.dds", 500);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the foliage object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the foliage object.
    if(m_Foliage)
    {
        m_Foliage->Shutdown();
        delete m_Foliage;
        m_Foliage = 0;
    }

    // Release the ground model object.
    if(m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = 0;
    }

    // Release the user interface object.
    if(m_UserInterface)
    {
        m_UserInterface->Shutdown();
        delete m_UserInterface;
        m_UserInterface = 0;
    }

    // Release the fps object.
    if(m_Fps)
    {
        delete m_Fps;
        m_Fps = 0;
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

    // Release the shader manager object.
    if(m_ShaderManager)
    {
        m_ShaderManager->Shutdown();
        delete m_ShaderManager;
        m_ShaderManager = 0;
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
    float posX, posY, posZ, rotX, rotY, rotZ;
    D3DXVECTOR3 cameraPosition;


    // Update the system stats.
    m_Timer->Frame();
    m_Fps->Frame();

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

    // Get the view point position/rotation.
    m_Position->GetPosition(posX, posY, posZ);
    m_Position->GetRotation(rotX, rotY, rotZ);

    // Do the frame processing for the user interface.
    result = m_UserInterface->Frame(m_Fps->GetFps(), posX, posY, posZ, rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Get the position of the camera.
    cameraPosition = m_Camera->GetPosition();

    // Do the frame processing for the foliage.
    result = m_Foliage->Frame(cameraPosition, m_Direct3D->GetDeviceContext());
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
    D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, baseViewMatrix;


    // Clear the scene.
    m_Direct3D->BeginScene(0.0f, 0.65f, 1.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the matrices from the camera and d3d objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);
    m_Camera->GetBaseViewMatrix(baseViewMatrix);

    // Render the ground model.
    m_GroundModel->Render(m_Direct3D->GetDeviceContext());
    m_ShaderManager->RenderTextureShader(m_Direct3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_GroundModel->GetColorTexture());

    // Turn on the alpha-to-coverage blending.
    m_Direct3D->EnableAlphaToCoverageBlending();

    // Render the foliage.
    m_Foliage->Render(m_Direct3D->GetDeviceContext());
    m_ShaderManager->RenderFoliageShader(m_Direct3D->GetDeviceContext(), m_Foliage->GetVertexCount(), m_Foliage->GetInstanceCount(), viewMatrix, projectionMatrix, m_Foliage->GetTexture());

    // Turn off the alpha blending.
    m_Direct3D->TurnOffAlphaBlending();

    // Render the user interface.
    m_UserInterface->Render(m_Direct3D, m_ShaderManager, worldMatrix, baseViewMatrix, orthoMatrix);

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}
