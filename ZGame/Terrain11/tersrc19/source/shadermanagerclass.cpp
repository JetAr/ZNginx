////////////////////////////////////////////////////////////////////////////////
// Filename: shadermanagerclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadermanagerclass.h"


ShaderManagerClass::ShaderManagerClass()
{
    m_TextureShader = 0;
    m_FontShader = 0;
    m_FoliageShader = 0;
}


ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}


ShaderManagerClass::~ShaderManagerClass()
{
}


bool ShaderManagerClass::Initialize(D3DClass* Direct3D, HWND hwnd)
{
    bool result;


    // Create the texture shader object.
    m_TextureShader = new TextureShaderClass;
    if(!m_TextureShader)
    {
        return false;
    }

    // Initialize the texture shader object.
    result = m_TextureShader->Initialize(Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the font shader object.
    m_FontShader = new FontShaderClass;
    if(!m_FontShader)
    {
        return false;
    }

    // Initialize the font shader object.
    result = m_FontShader->Initialize(Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // Create the foliage shader object.
    m_FoliageShader = new FoliageShaderClass;
    if(!m_FoliageShader)
    {
        return false;
    }

    // Initialize the foliage shader object.
    result = m_FoliageShader->Initialize(Direct3D->GetDevice(), hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the foliage shader object.", L"Error", MB_OK);
        return false;
    }

    return true;
}


void ShaderManagerClass::Shutdown()
{
    // Release the foliage shader object.
    if(m_FoliageShader)
    {
        m_FoliageShader->Shutdown();
        delete m_FoliageShader;
        m_FoliageShader = 0;
    }

    // Release the font shader object.
    if(m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
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


void ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    m_TextureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
    return;
}


void ShaderManagerClass::RenderFontShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR4 color)
{
    m_FontShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, color);
    return;
}


void ShaderManagerClass::RenderFoliageShader(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    m_FoliageShader->Render(deviceContext, vertexCount, instanceCount, viewMatrix, projectionMatrix, texture);
    return;
}
