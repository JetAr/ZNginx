////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"


ApplicationClass::ApplicationClass()
{
    m_Input = 0;
    m_D3D = 0;
    m_Timer = 0;
    m_Position = 0;
    m_Camera = 0;
    m_Light = 0;
    m_GroundModel = 0;
    m_Tree = 0;
    m_RenderTexture = 0;
    m_DepthShader = 0;
    m_TransparentDepthShader = 0;
    m_ShadowShader = 0;
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


    // Create the input object.  This object will be used to handle reading the keyboard input from the user.
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
    m_D3D = new D3DClass;
    if(!m_D3D)
    {
        return false;
    }

    // Initialize the Direct3D object.
    result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
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

    // Initialize the position object.
    m_Position = new PositionClass;
    if(!m_Position)
    {
        return false;
    }

    // Set the initial position.
    m_Position->SetPosition(0.0f, 7.0f, -11.0f);
    m_Position->SetRotation(20.0f, 0.0f, 0.0f);

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
    m_Light->GenerateOrthoMatrix(15.0f, 15.0f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);

    // Create the ground model object.
    m_GroundModel = new ModelClass;
    if(!m_GroundModel)
    {
        return false;
    }

    // Initialize the ground model object.
    result = m_GroundModel->Initialize(m_D3D->GetDevice(), "../Engine/data/plane01.txt", L"../Engine/data/dirt.dds", 2.0f);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
        return false;
    }

    // Set the position for the ground model.
    m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

    // Create the tree object.
    m_Tree = new TreeClass;
    if(!m_Tree)
    {
        return false;
    }

    // Initialize the shadow shader object.
    result = m_Tree->Initialize(m_D3D->GetDevice(), "../Engine/data/trees/trunk001.txt", L"../Engine/data/trees/trunk001.dds", "../Engine/data/trees/leaf001.txt", L"../Engine/data/trees/leaf001.dds", 0.1f);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
        return false;
    }

    // Set the position for the tree model.
    m_Tree->SetPosition(0.0f, 1.0f, 0.0f);

    // Create the render to texture object.
    m_RenderTexture = new RenderTextureClass;
    if(!m_RenderTexture)
    {
        return false;
    }

    // Initialize the render to texture object.
    result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SHADOWMAP_DEPTH, SHADOWMAP_NEAR);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
        return false;
    }

    // Create the depth shader object.
    m_DepthShader = new DepthShaderClass;
    if(!m_DepthShader)
    {
        return false;
    }

    // Initialize the depth shader object.
    result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the transparent depth shader object.
    m_TransparentDepthShader = new TransparentDepthShaderClass;
    if(!m_TransparentDepthShader)
    {
        return false;
    }

    // Initialize the transparent depth shader object.
    result = m_TransparentDepthShader->Initialize(m_D3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the transparent depth shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the shadow shader object.
    m_ShadowShader = new ShadowShaderClass;
    if(!m_ShadowShader)
    {
        return false;
    }

    // Initialize the shadow shader object.
    result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ApplicationClass::Shutdown()
{
    // Release the shadow shader object.
    if(m_ShadowShader)
    {
        m_ShadowShader->Shutdown();
        delete m_ShadowShader;
        m_ShadowShader = 0;
    }

    // Release the transparent depth shader object.
    if(m_TransparentDepthShader)
    {
        m_TransparentDepthShader->Shutdown();
        delete m_TransparentDepthShader;
        m_TransparentDepthShader = 0;
    }

    // Release the depth shader object.
    if(m_DepthShader)
    {
        m_DepthShader->Shutdown();
        delete m_DepthShader;
        m_DepthShader = 0;
    }

    // Release the render to texture object.
    if(m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        delete m_RenderTexture;
        m_RenderTexture = 0;
    }

    // Release the tree object.
    if(m_Tree)
    {
        m_Tree->Shutdown();
        delete m_Tree;
        m_Tree = 0;
    }

    // Release the ground model object.
    if(m_GroundModel)
    {
        m_GroundModel->Shutdown();
        delete m_GroundModel;
        m_GroundModel = 0;
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

    // Release the D3D object.
    if(m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = 0;
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

    // Do the input frame processing.
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

    // Update the movement of the camera location based on the user input.
    HandleMovementInput();

    // Update the scene lighting.
    UpdateLighting();

    // Render the graphics.
    result = Render();
    if(!result)
    {
        return false;
    }

    return true;
}


void ApplicationClass::HandleMovementInput()
{
    bool keyDown;
    float posX, posY, posZ, rotX, rotY, rotZ;


    // Set the frame time for calculating the updated position.
    m_Position->SetFrameTime(m_Timer->GetTime());

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

    return;
}


void ApplicationClass::UpdateLighting()
{
    static float angle = 270.0f;
    float radians;
    static float offsetX = 9.0f;


    // Update direction of the light.
    angle -= 0.03f * m_Timer->GetTime();
    if(angle < 90.0f)
    {
        angle = 270.0f;
        offsetX = 9.0f;
    }
    radians = angle * 0.0174532925f;
    m_Light->SetDirection(sinf(radians), cosf(radians), 0.0f);

    // Update the lookat and position.
    offsetX -= 0.003f * m_Timer->GetTime();
    m_Light->SetPosition(0.0f + offsetX, 10.0f, 1.0f);
    m_Light->SetLookAt(0.0f - offsetX, 0.0f, 2.0f);

    return;
}


bool ApplicationClass::Render()
{
    D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightOrthoMatrix;
    D3DXVECTOR4 ambientColor, diffuseColor;
    float posX, posY, posZ;


    // Render the depth of the scene to a texture.
    RenderSceneToTexture();

    // Clear the scene.
    m_D3D->BeginScene(0.0f, 0.5f, 0.8f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Generate the light view matrix based on the light's position.
    m_Light->GenerateViewMatrix();

    // Get the matrices from the camera and d3d objects.
    m_D3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);

    // Get the light's view and projection matrices from the light object.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // Set the light color attributes.
    diffuseColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
    ambientColor = D3DXVECTOR4(0.15f, 0.15f, 0.15f, 1.0f);

    // Translate to the position of the ground model.
    m_GroundModel->GetPosition(posX, posY, posZ);
    D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

    // Render the ground model using the shadow shader.
    m_GroundModel->Render(m_D3D->GetDevice());
    m_ShadowShader->Render(m_D3D->GetDevice(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
                           lightOrthoMatrix, m_GroundModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
                           ambientColor, diffuseColor, m_Light->GetDirection());
    m_D3D->GetWorldMatrix(worldMatrix);

    // Translate to the position of the tree model.
    m_Tree->GetPosition(posX, posY, posZ);
    D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

    // Render the tree trunk.
    m_Tree->RenderTrunk(m_D3D->GetDevice());
    m_ShadowShader->Render(m_D3D->GetDevice(), m_Tree->GetTrunkIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
                           lightOrthoMatrix, m_Tree->GetTrunkTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
                           ambientColor, diffuseColor, m_Light->GetDirection());

    // Enable blending and render the tree leaves.
    m_D3D->EnableAlphaBlending();
    m_Tree->RenderLeaves(m_D3D->GetDevice());
    m_ShadowShader->Render(m_D3D->GetDevice(), m_Tree->GetLeafIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
                           lightOrthoMatrix, m_Tree->GetLeafTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetPosition(),
                           ambientColor, diffuseColor, m_Light->GetDirection());
    m_D3D->DisableAlphaBlending();
    m_D3D->GetWorldMatrix(worldMatrix);

    // Present the rendered scene to the screen.
    m_D3D->EndScene();

    return true;
}


void ApplicationClass::RenderSceneToTexture()
{
    D3DXMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;
    float posX, posY, posZ;

    // Set the render target to be the render to texture.
    m_RenderTexture->SetRenderTarget(m_D3D->GetDevice());

    // Clear the render to texture.
    m_RenderTexture->ClearRenderTarget(m_D3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

    // Get the world matrix from the d3d object.
    m_D3D->GetWorldMatrix(worldMatrix);

    // Generate the light view matrix based on the light's position.
    m_Light->GenerateViewMatrix();

    // Get the view and orthographic matrices from the light object.
    m_Light->GetViewMatrix(lightViewMatrix);
    m_Light->GetOrthoMatrix(lightOrthoMatrix);

    // Translate to the position of the tree.
    m_Tree->GetPosition(posX, posY, posZ);
    D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

    // Render the tree trunk with the depth shader.
    m_Tree->RenderTrunk(m_D3D->GetDevice());
    m_DepthShader->Render(m_D3D->GetDevice(), m_Tree->GetTrunkIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

    // Render the tree leaves by turning on blending and then using the depth transparency shader.
    m_D3D->EnableAlphaBlending();
    m_Tree->RenderLeaves(m_D3D->GetDevice());
    m_TransparentDepthShader->Render(m_D3D->GetDevice(), m_Tree->GetLeafIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix, m_Tree->GetLeafTexture());
    m_D3D->DisableAlphaBlending();

    // Reset the world matrix.
    m_D3D->GetWorldMatrix(worldMatrix);

    // Translate to the position of the ground model.
    m_GroundModel->GetPosition(posX, posY, posZ);
    D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);

    // Render the ground model with the depth shader.
    m_GroundModel->Render(m_D3D->GetDevice());
    m_DepthShader->Render(m_D3D->GetDevice(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix);

    // Reset the render target back to the original back buffer and not the render to texture anymore.
    m_D3D->SetBackBufferRenderTarget();

    // Reset the viewport back to the original.
    m_D3D->ResetViewport();

    return;
}
