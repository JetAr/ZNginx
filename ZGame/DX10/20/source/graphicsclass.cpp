﻿////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
    m_D3D = 0;
    m_Camera = 0;
    m_Model = 0;
    m_BumpMapShader = 0;
    m_Light = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
    bool result;
    D3DXMATRIX baseViewMatrix;


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

    // Create the model object.
    m_Model = new ModelClass;
    if(!m_Model)
    {
        return false;
    }

    // Initialize the model object.
    result = m_Model->Initialize(m_D3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/stone01.dds",
                                 L"../Engine/data/bump01.dds");
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    // Create the bump map shader object.
    m_BumpMapShader = new BumpMapShaderClass;
    if(!m_BumpMapShader)
    {
        return false;
    }

    // Initialize the bump map shader object.
    result = m_BumpMapShader->Initialize(m_D3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the light object.
    m_Light = new LightClass;
    if(!m_Light)
    {
        return false;
    }

    // Initialize the light object.
    m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_Light->SetDirection(0.0f, 0.0f, 1.0f);

    return true;
}


void GraphicsClass::Shutdown()
{
    // Release the light object.
    if(m_Light)
    {
        delete m_Light;
        m_Light = 0;
    }

    // Release the bump map shader object.
    if(m_BumpMapShader)
    {
        m_BumpMapShader->Shutdown();
        delete m_BumpMapShader;
        m_BumpMapShader = 0;
    }

    // Release the model object.
    if(m_Model)
    {
        m_Model->Shutdown();
        delete m_Model;
        m_Model = 0;
    }

    // Release the camera object.
    if(m_Camera)
    {
        delete m_Camera;
        m_Camera = 0;
    }

    // Release the Direct3D object.
    if(m_D3D)
    {
        m_D3D->Shutdown();
        delete m_D3D;
        m_D3D = 0;
    }

    return;
}


bool GraphicsClass::Frame()
{
    // Set the position of the camera.
    m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

    return true;
}


bool GraphicsClass::Render()
{
    D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
    static float rotation = 0.0f;


    // Clear the buffers to begin the scene.
    m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // Generate the view matrix based on the camera's position.
    m_Camera->Render();

    // Get the world, view, projection, and ortho matrices from the camera and d3d objects.
    m_D3D->GetWorldMatrix(worldMatrix);
    m_Camera->GetViewMatrix(viewMatrix);
    m_D3D->GetProjectionMatrix(projectionMatrix);
    m_D3D->GetOrthoMatrix(orthoMatrix);

    // Update the rotation variable each frame.
    rotation += (float)D3DX_PI * 0.0025f;
    if(rotation > 360.0f)
    {
        rotation -= 360.0f;
    }

    // Rotate the world matrix by the rotation value.
    D3DXMatrixRotationY(&worldMatrix, rotation);

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
    m_Model->Render(m_D3D->GetDevice());

    // Render the model using the bump map shader.
    m_BumpMapShader->Render(m_D3D->GetDevice(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
                            m_Model->GetTextureArray(), m_Light->GetDirection(), m_Light->GetDiffuseColor());

    // Present the rendered scene to the screen.
    m_D3D->EndScene();

    return true;
}
