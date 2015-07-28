﻿////////////////////////////////////////////////////////////////////////////////
// Filename: bumpmodelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "bumpmodelclass.h"


BumpModelClass::BumpModelClass()
{
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_model = 0;
    m_ColorTexture = 0;
    m_NormalMapTexture = 0;
}


BumpModelClass::BumpModelClass(const BumpModelClass& other)
{
}


BumpModelClass::~BumpModelClass()
{
}


bool BumpModelClass::Initialize(ID3D10Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
    bool result;


    // Load in the model data.
    result = LoadModel(modelFilename);
    if(!result)
    {
        return false;
    }

    // Calculate the normal, tangent, and binormal vectors for the model.
    CalculateModelVectors();

    // Initialize the vertex and index buffer that hold the geometry for the model.
    result = InitializeBuffers(device);
    if(!result)
    {
        return false;
    }

    // Load the textures for this model.
    result = LoadTextures(device, textureFilename1, textureFilename2);
    if(!result)
    {
        return false;
    }

    return true;
}


void BumpModelClass::Shutdown()
{
    // Release the model textures.
    ReleaseTextures();

    // Release the vertex and index buffers.
    ShutdownBuffers();

    // Release the model data.
    ReleaseModel();

    return;
}


void BumpModelClass::Render(ID3D10Device* device)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(device);

    return;
}


int BumpModelClass::GetIndexCount()
{
    return m_indexCount;
}


ID3D10ShaderResourceView* BumpModelClass::GetColorTexture()
{
    return m_ColorTexture->GetTexture();
}


ID3D10ShaderResourceView* BumpModelClass::GetNormalMapTexture()
{
    return m_NormalMapTexture->GetTexture();
}


bool BumpModelClass::InitializeBuffers(ID3D10Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D10_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;
    int i;


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

    // Load the vertex array and index array with data.
    for(i=0; i<m_vertexCount; i++)
    {
        vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
        vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
        vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
        vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

        indices[i] = i;
    }

    // Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;

    // Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    // Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;

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


void BumpModelClass::ShutdownBuffers()
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


void BumpModelClass::RenderBuffers(ID3D10Device* device)
{
    unsigned int stride;
    unsigned int offset;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the vertex buffer to active in the input assembler so it can be rendered.
    device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}


bool BumpModelClass::LoadTextures(ID3D10Device* device, WCHAR* filename1, WCHAR* filename2)
{
    bool result;


    // Create the color texture object.
    m_ColorTexture = new TextureClass;
    if(!m_ColorTexture)
    {
        return false;
    }

    // Initialize the color texture object.
    result = m_ColorTexture->Initialize(device, filename1);
    if(!result)
    {
        return false;
    }

    // Create the normal map texture object.
    m_NormalMapTexture = new TextureClass;
    if(!m_NormalMapTexture)
    {
        return false;
    }

    // Initialize the normal map texture object.
    result = m_NormalMapTexture->Initialize(device, filename2);
    if(!result)
    {
        return false;
    }

    return true;
}


void BumpModelClass::ReleaseTextures()
{
    // Release the texture objects.
    if(m_ColorTexture)
    {
        m_ColorTexture->Shutdown();
        delete m_ColorTexture;
        m_ColorTexture = 0;
    }

    if(m_NormalMapTexture)
    {
        m_NormalMapTexture->Shutdown();
        delete m_NormalMapTexture;
        m_NormalMapTexture = 0;
    }

    return;
}


bool BumpModelClass::LoadModel(char* filename)
{
    ifstream fin;
    char input;
    int i;


    // Open the model file.  If it could not open the file then exit.
    fin.open(filename);
    if(fin.fail())
    {
        return false;
    }

    // Read up to the value of vertex count.
    fin.get(input);
    while(input != ':')
    {
        fin.get(input);
    }

    // Read in the vertex count.
    fin >> m_vertexCount;

    // Set the number of indices to be the same as the vertex count.
    m_indexCount = m_vertexCount;

    // Create the model using the vertex count that was read in.
    m_model = new ModelType[m_vertexCount];
    if(!m_model)
    {
        return false;
    }

    // Read up to the beginning of the data.
    fin.get(input);
    while(input != ':')
    {
        fin.get(input);
    }
    fin.get(input);
    fin.get(input);

    // Read in the vertex data.
    for(i=0; i<m_vertexCount; i++)
    {
        fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
        fin >> m_model[i].tu >> m_model[i].tv;
        fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
    }

    // Close the model file.
    fin.close();

    return true;
}


void BumpModelClass::ReleaseModel()
{
    if(m_model)
    {
        delete [] m_model;
        m_model = 0;
    }

    return;
}


void BumpModelClass::CalculateModelVectors()
{
    int faceCount, i, index;
    TempVertexType vertex1, vertex2, vertex3;
    VectorType tangent, binormal;


    // Calculate the number of faces in the model.
    faceCount = m_vertexCount / 3;

    // Initialize the index to the model data.
    index = 0;

    // Go through all the faces and calculate the the tangent, binormal, and normal vectors.
    for(i=0; i<faceCount; i++)
    {
        // Get the three vertices for this face from the model.
        vertex1.x = m_model[index].x;
        vertex1.y = m_model[index].y;
        vertex1.z = m_model[index].z;
        vertex1.tu = m_model[index].tu;
        vertex1.tv = m_model[index].tv;
        vertex1.nx = m_model[index].nx;
        vertex1.ny = m_model[index].ny;
        vertex1.nz = m_model[index].nz;
        index++;

        vertex2.x = m_model[index].x;
        vertex2.y = m_model[index].y;
        vertex2.z = m_model[index].z;
        vertex2.tu = m_model[index].tu;
        vertex2.tv = m_model[index].tv;
        vertex2.nx = m_model[index].nx;
        vertex2.ny = m_model[index].ny;
        vertex2.nz = m_model[index].nz;
        index++;

        vertex3.x = m_model[index].x;
        vertex3.y = m_model[index].y;
        vertex3.z = m_model[index].z;
        vertex3.tu = m_model[index].tu;
        vertex3.tv = m_model[index].tv;
        vertex3.nx = m_model[index].nx;
        vertex3.ny = m_model[index].ny;
        vertex3.nz = m_model[index].nz;
        index++;

        // Calculate the tangent and binormal of that face.
        CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

        // Store the tangent and binormal for this face back in the model structure.
        m_model[index-1].tx = tangent.x;
        m_model[index-1].ty = tangent.y;
        m_model[index-1].tz = tangent.z;
        m_model[index-1].bx = binormal.x;
        m_model[index-1].by = binormal.y;
        m_model[index-1].bz = binormal.z;

        m_model[index-2].tx = tangent.x;
        m_model[index-2].ty = tangent.y;
        m_model[index-2].tz = tangent.z;
        m_model[index-2].bx = binormal.x;
        m_model[index-2].by = binormal.y;
        m_model[index-2].bz = binormal.z;

        m_model[index-3].tx = tangent.x;
        m_model[index-3].ty = tangent.y;
        m_model[index-3].tz = tangent.z;
        m_model[index-3].bx = binormal.x;
        m_model[index-3].by = binormal.y;
        m_model[index-3].bz = binormal.z;
    }

    return;
}


void BumpModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
        VectorType& tangent, VectorType& binormal)
{
    float vector1[3], vector2[3];
    float tuVector[2], tvVector[2];
    float den;
    float length;


    // Calculate the two vectors for this face.
    vector1[0] = vertex2.x - vertex1.x;
    vector1[1] = vertex2.y - vertex1.y;
    vector1[2] = vertex2.z - vertex1.z;

    vector2[0] = vertex3.x - vertex1.x;
    vector2[1] = vertex3.y - vertex1.y;
    vector2[2] = vertex3.z - vertex1.z;

    // Calculate the tu and tv texture space vectors.
    tuVector[0] = vertex2.tu - vertex1.tu;
    tvVector[0] = vertex2.tv - vertex1.tv;

    tuVector[1] = vertex3.tu - vertex1.tu;
    tvVector[1] = vertex3.tv - vertex1.tv;

    // Calculate the denominator of the tangent/binormal equation.
    den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

    // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
    tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
    tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
    tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

    binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
    binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
    binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

    // Calculate the length of this normal.
    length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

    // Normalize the normal and then store it
    tangent.x = tangent.x / length;
    tangent.y = tangent.y / length;
    tangent.z = tangent.z / length;

    // Calculate the length of this normal.
    length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

    // Normalize the normal and then store it
    binormal.x = binormal.x / length;
    binormal.y = binormal.y / length;
    binormal.z = binormal.z / length;

    return;
}
