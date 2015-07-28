///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"


TextClass::TextClass()
{
    m_Font = 0;
    m_FontShader = 0;
    m_sentence1 = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D10Device* device, HWND hwnd, int screenWidth, int screenHeight, D3DXMATRIX baseViewMatrix)
{
    bool result;


    // Store the screen width and height.
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    // Store the base view matrix.
    m_baseViewMatrix = baseViewMatrix;

    // Create the font object.
    m_Font = new FontClass;
    if(!m_Font)
    {
        return false;
    }

    // Initialize the font object.
    result = m_Font->Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
        return false;
    }

    // Create the font shader object.
    m_FontShader = new FontShaderClass;
    if(!m_FontShader)
    {
        return false;
    }

    // Initialize the font shader object.
    result = m_FontShader->Initialize(device, hwnd);
    if(!result)
    {
        MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
        return false;
    }

    // Initialize the first sentence.
    result = InitializeSentence(&m_sentence1, 32, device);
    if(!result)
    {
        return false;
    }

    // Now update the sentence vertex buffer with the new string information.
    result = UpdateSentence(m_sentence1, "Intersection: No", 20, 20, 1.0f, 0.0f, 0.0f);
    if(!result)
    {
        return false;
    }

    return true;
}


void TextClass::Shutdown()
{
    // Release the sentence.
    ReleaseSentence(&m_sentence1);

    // Release the font shader object.
    if(m_FontShader)
    {
        m_FontShader->Shutdown();
        delete m_FontShader;
        m_FontShader = 0;
    }

    // Release the font object.
    if(m_Font)
    {
        m_Font->Shutdown();
        delete m_Font;
        m_Font = 0;
    }

    return;
}


void TextClass::Render(ID3D10Device* device, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
    // Draw the sentence.
    RenderSentence(device, m_sentence1, worldMatrix, orthoMatrix);

    return;
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D10Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D10_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;


    // Create a new sentence object.
    *sentence = new SentenceType;
    if(!*sentence)
    {
        return false;
    }

    // Initialize the sentence buffers to null.
    (*sentence)->vertexBuffer = 0;
    (*sentence)->indexBuffer = 0;

    // Set the maximum length of the sentence.
    (*sentence)->maxLength = maxLength;

    // Set the number of vertices in the vertex array.
    (*sentence)->vertexCount = 6 * maxLength;

    // Set the number of indexes in the index array.
    (*sentence)->indexCount = (*sentence)->vertexCount;

    // Create the vertex array.
    vertices = new VertexType[(*sentence)->vertexCount];
    if(!vertices)
    {
        return false;
    }

    // Create the index array.
    indices = new unsigned long[(*sentence)->indexCount];
    if(!indices)
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

    // Initialize the index array.
    for(i=0; i<(*sentence)->indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;

    // Create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;

    // Create the index buffer.
    result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Release the vertex array as it is no longer needed.
    delete [] vertices;
    vertices = 0;

    // Release the index array as it is no longer needed.
    delete [] indices;
    indices = 0;

    return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue)
{
    int numLetters;
    VertexType* vertices;
    float drawX, drawY;
    void* verticesPtr;
    HRESULT result;


    // Store the color of the sentence.
    sentence->red = red;
    sentence->green = green;
    sentence->blue = blue;

    // Get the number of letters in the sentence.
    numLetters = (int)strlen(text);

    // Check for possible buffer overflow.
    if(numLetters > sentence->maxLength)
    {
        return false;
    }

    // Create the vertex array.
    vertices = new VertexType[sentence->vertexCount];
    if(!vertices)
    {
        return false;
    }

    // Initialize vertex array to zeros at first.
    memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

    // Calculate the X and Y pixel position on the screen to start drawing to.
    drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
    drawY = (float)((m_screenHeight / 2) - positionY);

    // Use the font class to build the vertex array from the sentence text and sentence draw location.
    m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

    // Initialize the vertex buffer pointer to null first.
    verticesPtr = 0;

    // Lock the vertex buffer.
    result = sentence->vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
    if(FAILED(result))
    {
        return false;
    }

    // Copy the vertex array into the vertex buffer.
    memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

    // Unlock the vertex buffer.
    sentence->vertexBuffer->Unmap();

    // Release the vertex array as it is no longer needed.
    delete [] vertices;
    vertices = 0;

    return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
    if(*sentence)
    {
        // Release the sentence vertex buffer.
        if((*sentence)->vertexBuffer)
        {
            (*sentence)->vertexBuffer->Release();
            (*sentence)->vertexBuffer = 0;
        }

        // Release the sentence index buffer.
        if((*sentence)->indexBuffer)
        {
            (*sentence)->indexBuffer->Release();
            (*sentence)->indexBuffer = 0;
        }

        // Release the sentence.
        delete *sentence;
        *sentence = 0;
    }

    return;
}


void TextClass::RenderSentence(ID3D10Device* device, SentenceType* sentence, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
    unsigned int stride, offset;
    D3DXVECTOR4 pixelColor;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    device->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    device->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Create a pixel color vector with the input sentence color.
    pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

    // Render the text using the font shader.
    m_FontShader->Render(device, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);

    return;
}


bool TextClass::SetIntersection(bool intersection)
{
    char intersectionString[32];
    bool result;


    if(intersection)
    {
        strcpy_s(intersectionString, "Intersection: Yes");
        result = UpdateSentence(m_sentence1, intersectionString, 20, 20, 0.0f, 1.0f, 0.0f);
    }
    else
    {
        strcpy_s(intersectionString, "Intersection: No");
        result = UpdateSentence(m_sentence1, intersectionString, 20, 20, 1.0f, 0.0f, 0.0f);
    }

    return result;
}
