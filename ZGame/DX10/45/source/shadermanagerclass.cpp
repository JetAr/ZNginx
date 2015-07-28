////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadermanagerclass.h"


ShaderManagerClass::ShaderManagerClass()
{
    m_TextureShader = 0;
    m_LightShader = 0;
    m_BumpMapShader = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
}


bool ShaderManagerClass::Initialize(ID3D10Device* device, HWND hwnd)
{
    bool result;


    // Create the texture shader object.
    m_TextureShader = new TextureShaderClass;
    if(!m_TextureShader)
    {
        return false;
    }

    // Initialize the texture shader object.
    result = m_TextureShader->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the light shader object.
    m_LightShader = new LightShaderClass;
    if(!m_LightShader)
    {
        return false;
    }

    // Initialize the light shader object.
    result = m_LightShader->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the bump map shader object.
    m_BumpMapShader = new BumpMapShaderClass;
    if(!m_BumpMapShader)
    {
        return false;
    }

    // Initialize the bump map shader object.
    result = m_BumpMapShader->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ShaderManagerClass::Shutdown()
{
    // Release the bump map shader object.
    if(m_BumpMapShader)
    {
        m_BumpMapShader->Shutdown();
        delete m_BumpMapShader;
        m_BumpMapShader = 0;
    }

    // Release the light shader object.
    if(m_LightShader)
    {
        m_LightShader->Shutdown();
        delete m_LightShader;
        m_LightShader = 0;
    }

    // Release the texture shader object.
    if(m_TextureShader)
    {
        m_TextureShader->Shutdown();
        delete m_TextureShader;
        m_TextureShader = 0;
    }

    return;
}


void ShaderManagerClass::RenderTextureShader(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
        ID3D10ShaderResourceView* texture)
{
    // Render the model using the texture shader.
    m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);

    return;
}


void ShaderManagerClass::RenderLightShader(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
        ID3D10ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambient, D3DXVECTOR4 diffuse,
        D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specular, float specularPower)
{
    // Render the model using the light shader.
    m_LightShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambient, diffuse, cameraPosition, specular,
                          specularPower);

    return;
}


void ShaderManagerClass::RenderBumpMapShader(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
        ID3D10ShaderResourceView* colorTexture, ID3D10ShaderResourceView* normalTexture, D3DXVECTOR3 lightDirection,
        D3DXVECTOR4 diffuse)
{
    // Render the model using the bump map shader.
    m_BumpMapShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuse);

    return;
}
