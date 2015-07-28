////////////////////////////////////////////////////////////////////////////////
// Filename: waterclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "waterclass.h"


WaterClass::WaterClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_Texture = 0;
}


WaterClass::WaterClass(const WaterClass& other)
{
}


WaterClass::~WaterClass()
{
}


bool WaterClass::Initialize(ID3D11Device* device, WCHAR* textureFilename, float waterHeight, float waterRadius)
{
    bool result;


    // Store the water height.
    m_waterHeight = waterHeight;

    // Initialize the vertex and index buffer that hold the geometry for the triangle.
    result = InitializeBuffers(device, waterRadius);
    if(!result)
    {
        return false;
    }

    // Load the texture for this model.
    result = LoadTexture(device, textureFilename);
    if(!result)
    {
        return false;
    }

    // Set the tiling for the water normal maps.
    m_normalMapTiling.x = 0.01f;  // Tile ten times over the quad.
    m_normalMapTiling.y = 0.02f;  // Tile five times over the quad.

    // Initialize the water translation to zero.
    m_waterTranslation = 0.0f;

    // Set the scaling value for the water normal map.
    m_reflectRefractScale = 0.03f;

    // Set the tint of the refraction.
    m_refractionTint = D3DXVECTOR4(0.0f, 0.8f, 1.0f, 1.0f);

    // Set the specular shininess.
    m_specularShininess = 200.0f;

    return true;
}


void WaterClass::Shutdown()
{
    // Release the model texture.
    ReleaseTexture();

    // Release the vertex and index buffers.
    ShutdownBuffers();

    return;
}


void WaterClass::Frame()
{
    // Update the position of the water to simulate motion.
    m_waterTranslation += 0.003f;
    if(m_waterTranslation > 1.0f)
    {
        m_waterTranslation -= 1.0f;
    }

    return;
}


void WaterClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return;
}


int WaterClass::GetIndexCount()
{
    return m_indexCount;
}


ID3D11ShaderResourceView* WaterClass::GetTexture()
{
    return m_Texture->GetTexture();
}


float WaterClass::GetWaterHeight()
{
    return m_waterHeight;
}


D3DXVECTOR2 WaterClass::GetNormalMapTiling()
{
    return m_normalMapTiling;
}


float WaterClass::GetWaterTranslation()
{
    return m_waterTranslation;
}


float WaterClass::GetReflectRefractScale()
{
    return m_reflectRefractScale;
}


D3DXVECTOR4 WaterClass::GetRefractionTint()
{
    return m_refractionTint;
}


float WaterClass::GetSpecularShininess()
{
    return m_specularShininess;
}


bool WaterClass::InitializeBuffers(ID3D11Device* device, float waterRadius)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;


    // Set the number of vertices in the vertex array.
    m_vertexCount = 6;

    // Set the number of indices in the index array.
    m_indexCount = 6;

    // Create the vertex array.
    vertices = new VertexType[m_vertexCount];
    if(!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new unsigned long[m_indexCount];
    if(!indices)
    {
        return false;
    }

    // Load the vertex array with data.
    vertices[0].position = D3DXVECTOR3(-waterRadius, 0.0f, waterRadius);  // Top left.
    vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

    vertices[1].position = D3DXVECTOR3(waterRadius, 0.0f, waterRadius);  // Top right.
    vertices[1].texture = D3DXVECTOR2(1.0f, 0.0f);

    vertices[2].position = D3DXVECTOR3(-waterRadius, 0.0f, -waterRadius);  // Bottom left.
    vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

    vertices[3].position = D3DXVECTOR3(-waterRadius, 0.0f, -waterRadius);  // Bottom left.
    vertices[3].texture = D3DXVECTOR2(0.0f, 1.0f);

    vertices[4].position = D3DXVECTOR3(waterRadius, 0.0f, waterRadius);  // Top right.
    vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

    vertices[5].position = D3DXVECTOR3(waterRadius, 0.0f, -waterRadius);  // Bottom right.
    vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

    // Load the index array with data.
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
    indices[4] = 4;
    indices[5] = 5;

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Release the arrays now that the vertex and index buffers have been created and loaded.
    delete [] vertices;
    vertices = 0;

    delete [] indices;
    indices = 0;

    return true;
}


void WaterClass::ShutdownBuffers()
{
    // Release the index buffer.
    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    // Release the vertex buffer.
    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    return;
}


void WaterClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}


bool WaterClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
    bool result;


    // Create the texture object.
    m_Texture = new TextureClass;
    if(!m_Texture)
    {
        return false;
    }

    // Initialize the texture object.
    result = m_Texture->Initialize(device, filename);
    if(!result)
    {
        return false;
    }

    return true;
}


void WaterClass::ReleaseTexture()
{
    // Release the texture object.
    if(m_Texture)
    {
        m_Texture->Shutdown();
        delete m_Texture;
        m_Texture = 0;
    }

    return;
}
