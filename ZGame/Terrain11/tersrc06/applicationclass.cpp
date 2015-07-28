////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
    m_Input = 0;
    m_Direct3D = 0;
    m_Camera = 0;
    m_Terrain = 0;
    m_Timer = 0;
    m_Position = 0;
    m_Fps = 0;
    m_Cpu = 0;
    m_FontShader = 0;
    m_Text = 0;
    m_TerrainShader = 0;
    m_Light = 0;
    m_Frustum = 0;
    m_QuadTree = 0;
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
    float cameraX, cameraY, cameraZ;
    D3DXMATRIX baseViewMatrix;
    char videoCard[128];
    int videoMemory;


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

    // Create the camera object.
    m_Camera = new CameraClass;
    if(!m_Camera)
    {
        return false;
    }

    // Initialize a base view matrix with the camera for 2D user interface rendering.
    m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
    m_Camera->Render();
    m_Camera->GetViewMatrix(baseViewMatrix);

    // Set the initial position of the camera.
    cameraX = 50.0f;
    cameraY = 2.0f;
    cameraZ = -7.0f;

    m_Camera->SetPosition(cameraX, cameraY, cameraZ);

    // Create the terrain object.
    m_Terrain = new TerrainClass;
    if(!m_Terrain)
    {
        return false;
    }

    // Initialize the terrain object.
    result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../Engine/data/heightmap01.bmp", L"../Engine/data/dirt01.dds");
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
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

    // Set the initial position of the viewer to the same as the initial camera position.
    m_Position->SetPosition(cameraX, cameraY, cameraZ);

    // Create the fps object.
    m_Fps = new FpsClass;
    if(!m_Fps)
    {
        return false;
    }

    // Initialize the fps object.
    m_Fps->Initialize();

    // Create the cpu object.
    m_Cpu = new CpuClass;
    if(!m_Cpu)
    {
        return false;
    }

    // Initialize the cpu object.
    m_Cpu->Initialize();

    // Create the font shader object.
    m_FontShader = new FontShaderClass;
    if(!m_FontShader)
    {
        return false;
    }

    // Initialize the font shader object.
    result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the text object.
    m_Text = new TextClass;
    if(!m_Text)
    {
        return false;
    }

    // Initialize the text object.
    result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
        return false;
    }

    // Retrieve the video card information.
    m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

    // Set the video card information in the text object.
    result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
    if(!result)
    {
        MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
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

    // Create the light object.
    m_Light = new LightClass;
    if(!m_Light)
    {
        return false;
    }

    // Initialize the light object.
    m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(-0.5f, -1.0f, 0.0f);

    // Create the frustum object.
    m_Frustum = new FrustumClass;
    if(!m_Frustum)
    {
        return false;
    }

    // Create the quad tree object.
    m_QuadTree = new QuadTreeClass;
    if(!m_QuadTree)
    {
        return false;
    }

    // Initialize the quad tree object.
    result = m_QuadTree->Initialize(m_Terrain, m_Direct3D->GetDevice());
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the quad tree object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the quad tree object.
    if(m_QuadTree)
    {
        m_QuadTree->Shutdown();
        delete m_QuadTree;
        m_QuadTree = 0;
    }

    // Release the frustum object.
    if(m_Frustum)
    {
        delete m_Frustum;
        m_Frustum = 0;
    }

    // Release the light object.
    if(m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // Release the terrain shader object.
    if(m_TerrainShader)
    {
        m_TerrainShader->Shutdown();
        delete m_TerrainShader;
        m_TerrainShader = 0;
    }

    // Release the text object.
    if(m_Text)
    {
        m_Text->Shutdown();
        delete m_Text;
        m_Text = 0;
    }

    // Release the font shader object.
    if(m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
    }

    // Release the cpu object.
    if(m_Cpu)
    {
        m_Cpu->Shutdown();
        delete m_Cpu;
        m_Cpu = 0;
    }

    // Release the fps object.
    if(m_Fps)
    {
        delete m_Fps;
        m_Fps = 0;
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

    // Release the terrain object.
    if(m_Terrain)
    {
        m_Terrain->Shutdown();
        delete m_Terrain;
        m_Terrain = 0;
    }

    // Release the camera object.
    if(m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
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
    bool result, foundHeight;
    D3DXVECTOR3 position;
    float height;


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

    // Update the system stats.
    m_Timer->Frame();
    m_Fps->Frame();
    m_Cpu->Frame();

    // Update the FPS value in the text object.
    result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Update the CPU usage value in the text object.
    result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Do the frame input processing.
    result = HandleInput(m_Timer->GetTime());
    if(!result)
    {
        return false;
    }

    // Get the current position of the camera.
    position = m_Camera->GetPosition();

    // Get the height of the triangle that is directly underneath the given camera position.
    foundHeight =  m_QuadTree->GetHeightAtPosition(position.x, position.z, height);
    if(foundHeight)
    {
        // If there was a triangle under the camera then position the camera just above it by two units.
        m_Camera->SetPosition(position.x, height + 2.0f, position.z);
    }

    // Render the graphics.
    result = RenderGraphics();
    if(!result)
    {
        return false;
    }

    return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
    bool keyDown, result;
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

    // Update the position values in the text object.
    result = m_Text->SetCameraPosition(posX, posY, posZ, m_Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Update the rotation values in the text object.
    result = m_Text->SetCameraRotation(rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    return true;
}


bool ApplicationClass::RenderGraphics()
{
    D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
    bool result;


    // Clear the scene.
    m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
    m_Direct3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_Direct3D->GetProjectionMatrix(projectionMatrix);
    m_Direct3D->GetOrthoMatrix(orthoMatrix);

    // Construct the frustum.
    m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);

    // Set the terrain shader parameters that it will use for rendering.
    result = m_TerrainShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Light->GetAmbientColor(),
             m_Light->GetDiffuseColor(), m_Light->GetDirection(), m_Terrain->GetTexture());
    if(!result)
    {
        return false;
    }

    // Render the terrain using the quad tree and terrain shader.
    m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);

    // Set the number of rendered terrain triangles since some were culled.
    result = m_Text->SetRenderCount(m_QuadTree->GetDrawCount(), m_Direct3D->GetDeviceContext());
    if(!result)
    {
        return false;
    }

    // Turn off the Z buffer to begin all 2D rendering.
    m_Direct3D->TurnZBufferOff();

    // Turn on the alpha blending before rendering the text.
    m_Direct3D->TurnOnAlphaBlending();

    // Render the text user interface elements.
    result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
    if(!result)
    {
        return false;
    }

    // Turn off alpha blending after rendering the text.
    m_Direct3D->TurnOffAlphaBlending();

    // Turn the Z buffer back on now that all 2D rendering has completed.
    m_Direct3D->TurnZBufferOn();

    // Present the rendered scene to the screen.
    m_Direct3D->EndScene();

    return true;
}
