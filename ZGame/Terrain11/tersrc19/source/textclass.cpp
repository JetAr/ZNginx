///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"


TextClass::TextClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_vertexBuffer2 = 0;
    m_indexBuffer2 = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D11Device* device, FontClass* Font, int screenWidth, int screenHeight, int maxLength, char* text, int positionX, int positionY, float red, float green, float blue, bool shadow,
                           ID3D11DeviceContext* deviceContext)
{
    bool result;


    // Store if this sentence is shadowed or not.
    m_shadow = shadow;

    // Store the screen width and height.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Store the maximum length of the sentence.
    m_maxLength = maxLength;

    // Initalize the sentence.
    result = InitializeSentence(device, Font, text, positionX, positionY, red, green, blue, deviceContext);
    if(!result)
    {
        return false;
    }

    return true;
}


void TextClass::Shutdown()
{
    // Release the buffers.
    if(m_vertexBuffer)
    {
        m_vertexBuffer->Release();
        m_vertexBuffer = 0;
    }

    if(m_indexBuffer)
    {
        m_indexBuffer->Release();
        m_indexBuffer = 0;
    }

    if(m_vertexBuffer2)
    {
        m_vertexBuffer2->Release();
        m_vertexBuffer2 = 0;
    }

    if(m_indexBuffer2)
    {
        m_indexBuffer2->Release();
        m_indexBuffer2 = 0;
    }

    return;
}


void TextClass::Render(ID3D11DeviceContext* deviceContext, ShaderManagerClass* ShaderManager, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture)
{
    // Draw the sentence.
    RenderSentence(deviceContext, ShaderManager, worldMatrix, viewMatrix, orthoMatrix, fontTexture);

    return;
}


bool TextClass::InitializeSentence(ID3D11Device* device, FontClass* Font, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;


    // Set the vertex and index count.
    m_vertexCount = 6 * m_maxLength;
    m_indexCount = 6 * m_maxLength;

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

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    // Initialize the index array.
    for(i=0; i<m_indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // Create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
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

    // If shadowed create the second vertex and index buffer.
    if(m_shadow)
    {
        result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer2);
        if(FAILED(result))
        {
            return false;
        }

        result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer2);
        if(FAILED(result))
        {
            return false;
        }
    }

    // Release the vertex array as it is no longer needed.
    delete [] vertices;
    vertices = 0;

    // Release the index array as it is no longer needed.
    delete [] indices;
    indices = 0;

    // Now add the text data to the sentence buffers.
    result = UpdateSentence(Font, text, positionX, positionY, red, green, blue, deviceContext);
    if(!result)
    {
        return false;
    }

    return true;
}


bool TextClass::UpdateSentence(FontClass* Font, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
    int numLetters;
    VertexType* vertices;
    float drawX, drawY;
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    VertexType* verticesPtr;


    // Store the color of the sentence.
    m_pixelColor = D3DXVECTOR4(red, green, blue, 1.0f);

    // Get the number of letters in the sentence.
    numLetters = (int)strlen(text);

    // Check for possible buffer overflow.
    if(numLetters > m_maxLength)
    {
        return false;
    }

    // Create the vertex array.
    vertices = new VertexType[m_vertexCount];
    if(!vertices)
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

    // Calculate the X and Y pixel position on the screen to start drawing to.
    drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
    drawY = (float)((m_screenHeight / 2) - positionY);

    // Use the font class to build the vertex array from the sentence text and sentence draw location.
    Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // Lock the vertex buffer so it can be written to.
    result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return false;
    }

    // Get a pointer to the data in the vertex buffer.
    verticesPtr = (VertexType*)mappedResource.pData;

    // Copy the data into the vertex buffer.
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

    // Unlock the vertex buffer.
    deviceContext->Unmap(m_vertexBuffer, 0);

    // If shadowed then do the same for the second vertex buffer but offset by one pixel.
    if(m_shadow)
    {
        memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

        drawX = (float)((((m_screenWidth / 2) * -1) + positionX) + 2);
        drawY = (float)(((m_screenHeight / 2) - positionY) - 2);
        Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

        result = deviceContext->Map(m_vertexBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        if(FAILED(result))
        {
            return false;
        }

        // Get a pointer to the data in the vertex buffer.
        verticesPtr = (VertexType*)mappedResource.pData;

        memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

        deviceContext->Unmap(m_vertexBuffer2, 0);
    }

    // Release the vertex array as it is no longer needed.
    delete [] vertices;
    vertices = 0;

    return true;
}


void TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, ShaderManagerClass* ShaderManager, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX orthoMatrix, ID3D11ShaderResourceView* fontTexture)
{
    unsigned int stride, offset;
    D3DXVECTOR4 shadowColor;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // If shadowed then render the shadow text first.
    if(m_shadow)
    {
        shadowColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
        deviceContext->IASetIndexBuffer(m_indexBuffer2, DXGI_FORMAT_R32_UINT, 0);
        deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer2, &stride, &offset);
        ShaderManager->RenderFontShader(deviceContext, m_indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, shadowColor);
    }

    // Render the text buffers.
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    ShaderManager->RenderFontShader(deviceContext, m_indexCount, worldMatrix, viewMatrix, orthoMatrix, fontTexture, m_pixelColor);

    return;
}
