////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"


TerrainClass::TerrainClass()
{
    m_heightMap = 0;
    m_model = 0;
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}


bool TerrainClass::Initialize(ID3D11Device* device, char* heightMapFilename, float maximumHeight)
{
    bool result;


    // Load in the height map for the terrain.
    result = LoadHeightMap(heightMapFilename);
    if(!result)
    {
        return false;
    }

    // Reduce the height of the height map.
    ReduceHeightMap(maximumHeight);

    // Calculate the normals for the terrain data.
    result = CalculateNormals();
    if(!result)
    {
        return false;
    }

    // Construct a 3D model from the height map and normal data.
    result = BuildModel();
    if(!result)
    {
        return false;
    }

    // Calculate the normal, tangent, and binormal vectors for the terrain model.
    CalculateModelVectors();

    // Initialize the vertex and index buffer that hold the geometry for the terrain.
    result = InitializeBuffers(device);
    if(!result)
    {
        return false;
    }

    // Release the height map and the model since the data is now loaded into the vertex and index buffers.
    ReleaseHeightMap();
    ReleaseModel();

    return true;
}


void TerrainClass::Shutdown()
{
    // Release the buffers.
    ReleaseBuffers();

    // Release the model.
    ReleaseModel();

    // Release the height map.
    ReleaseHeightMap();

    return;
}


void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(deviceContext);

    return;
}


int TerrainClass::GetIndexCount()
{
    return m_indexCount;
}


bool TerrainClass::LoadHeightMap(char* filename)
{
    FILE* filePtr;
    int error;
    unsigned int count;
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;
    int imageSize, i, j, k, index;
    unsigned char* bitmapImage;
    unsigned char height;


    // Open the height map file in binary.
    error = fopen_s(&filePtr, filename, "rb");
    if(error != 0)
    {
        return false;
    }

    // Read in the file header.
    count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
    if(count != 1)
    {
        return false;
    }

    // Read in the bitmap info header.
    count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    if(count != 1)
    {
        return false;
    }

    // Save the dimensions of the terrain.
    m_terrainWidth = bitmapInfoHeader.biWidth;
    m_terrainHeight = bitmapInfoHeader.biHeight;

    // Calculate the size of the bitmap image data.
    imageSize = m_terrainWidth * m_terrainHeight * 3;

    // Allocate memory for the bitmap image data.
    bitmapImage = new unsigned char[imageSize];
    if(!bitmapImage)
    {
        return false;
    }

    // Move to the beginning of the bitmap data.
    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

    // Read in the bitmap image data.
    count = fread(bitmapImage, 1, imageSize, filePtr);
    if(count != imageSize)
    {
        return false;
    }

    // Close the file.
    error = fclose(filePtr);
    if(error != 0)
    {
        return false;
    }

    // Create the structure to hold the height map data.
    m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
    if(!m_heightMap)
    {
        return false;
    }

    // Initialize the position in the image data buffer.
    k=0;

    // Read the image data into the height map.
    for(j=0; j<m_terrainHeight; j++)
    {
        for(i=0; i<m_terrainWidth; i++)
        {
            height = bitmapImage[k];

            index = (m_terrainWidth * j) + i;

            m_heightMap[index].x = (float)i;
            m_heightMap[index].y = (float)height;
            m_heightMap[index].z = (float)j;

            k+=3;
        }
    }

    // Release the bitmap image data.
    delete [] bitmapImage;
    bitmapImage = 0;

    return true;
}


void TerrainClass::ReduceHeightMap(float value)
{
    int i, j;


    for(j=0; j<m_terrainHeight; j++)
    {
        for(i=0; i<m_terrainWidth; i++)
        {
            m_heightMap[(m_terrainWidth * j) + i].y /= value;
        }
    }

    return;
}


bool TerrainClass::CalculateNormals()
{
    int i, j, index1, index2, index3, index, count;
    float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
    VectorType* normals;


    // Create a temporary array to hold the un-normalized normal vectors.
    normals = new VectorType[(m_terrainHeight-1) * (m_terrainWidth-1)];
    if(!normals)
    {
        return false;
    }

    // Go through all the faces in the mesh and calculate their normals.
    for(j=0; j<(m_terrainHeight-1); j++)
    {
        for(i=0; i<(m_terrainWidth-1); i++)
        {
            index1 = (j * m_terrainWidth) + i;
            index2 = (j * m_terrainWidth) + (i+1);
            index3 = ((j+1) * m_terrainWidth) + i;

            // Get three vertices from the face.
            vertex1[0] = m_heightMap[index1].x;
            vertex1[1] = m_heightMap[index1].y;
            vertex1[2] = m_heightMap[index1].z;

            vertex2[0] = m_heightMap[index2].x;
            vertex2[1] = m_heightMap[index2].y;
            vertex2[2] = m_heightMap[index2].z;

            vertex3[0] = m_heightMap[index3].x;
            vertex3[1] = m_heightMap[index3].y;
            vertex3[2] = m_heightMap[index3].z;

            // Calculate the two vectors for this face.
            vector1[0] = vertex1[0] - vertex3[0];
            vector1[1] = vertex1[1] - vertex3[1];
            vector1[2] = vertex1[2] - vertex3[2];
            vector2[0] = vertex3[0] - vertex2[0];
            vector2[1] = vertex3[1] - vertex2[1];
            vector2[2] = vertex3[2] - vertex2[2];

            index = (j * (m_terrainWidth-1)) + i;

            // Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
            normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
            normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
            normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
        }
    }

    // Now go through all the vertices and take an average of each face normal that the vertex touches to get the averaged normal for that vertex.
    for(j=0; j<m_terrainHeight; j++)
    {
        for(i=0; i<m_terrainWidth; i++)
        {
            // Initialize the sum.
            sum[0] = 0.0f;
            sum[1] = 0.0f;
            sum[2] = 0.0f;

            // Initialize the count.
            count = 0;

            // Bottom left face.
            if(((i-1) >= 0) && ((j-1) >= 0))
            {
                index = ((j-1) * (m_terrainWidth-1)) + (i-1);

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Bottom right face.
            if((i < (m_terrainWidth-1)) && ((j-1) >= 0))
            {
                index = ((j-1) * (m_terrainWidth-1)) + i;

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Upper left face.
            if(((i-1) >= 0) && (j < (m_terrainHeight-1)))
            {
                index = (j * (m_terrainWidth-1)) + (i-1);

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Upper right face.
            if((i < (m_terrainWidth-1)) && (j < (m_terrainHeight-1)))
            {
                index = (j * (m_terrainWidth-1)) + i;

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Take the average of the faces touching this vertex.
            sum[0] = (sum[0] / (float)count);
            sum[1] = (sum[1] / (float)count);
            sum[2] = (sum[2] / (float)count);

            // Calculate the length of this normal.
            length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

            // Get an index to the vertex location in the height map array.
            index = (j * m_terrainWidth) + i;

            // Normalize the final shared normal for this vertex and store it in the height map array.
            m_heightMap[index].nx = (sum[0] / length);
            m_heightMap[index].ny = (sum[1] / length);
            m_heightMap[index].nz = (sum[2] / length);
        }
    }

    // Release the temporary normals.
    delete [] normals;
    normals = 0;

    return true;
}


bool TerrainClass::BuildModel()
{
    int i, j, index, index1, index2, index3, index4;
    float incrementSize, tu2Left, tu2Right, tv2Top, tv2Bottom;


    // Set the number of vertices in the model.
    m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

    // Create the terrain model array.
    m_model = new ModelType[m_vertexCount];
    if(!m_model)
    {
        return false;
    }

    // Setup the increment size for the second set of textures (alpha map).
    incrementSize = 1.0f / 31.0f;

    // Initialize the texture increments.
    tu2Left = 0.0f;
    tu2Right = incrementSize;
    tv2Bottom = 1.0f;
    tv2Top = 1.0f - incrementSize;

    // Load the terrain model with the height map terrain data.
    index = 0;

    for(j=0; j<(m_terrainHeight-1); j++)
    {
        for(i=0; i<(m_terrainWidth-1); i++)
        {
            index1 = (m_terrainWidth * j) + i;          // Bottom left.
            index2 = (m_terrainWidth * j) + (i+1);      // Bottom right.
            index3 = (m_terrainWidth * (j+1)) + i;      // Upper left.
            index4 = (m_terrainWidth * (j+1)) + (i+1);  // Upper right.

            // Upper left.
            m_model[index].x  = m_heightMap[index3].x;
            m_model[index].y  = m_heightMap[index3].y;
            m_model[index].z  = m_heightMap[index3].z;
            m_model[index].nx = m_heightMap[index3].nx;
            m_model[index].ny = m_heightMap[index3].ny;
            m_model[index].nz = m_heightMap[index3].nz;
            m_model[index].tu = 0.0f;
            m_model[index].tv = 0.0f;
            m_model[index].tu2 = tu2Left;
            m_model[index].tv2 = tv2Top;
            index++;

            // Upper right.
            m_model[index].x  = m_heightMap[index4].x;
            m_model[index].y  = m_heightMap[index4].y;
            m_model[index].z  = m_heightMap[index4].z;
            m_model[index].nx = m_heightMap[index4].nx;
            m_model[index].ny = m_heightMap[index4].ny;
            m_model[index].nz = m_heightMap[index4].nz;
            m_model[index].tu = 1.0f;
            m_model[index].tv = 0.0f;
            m_model[index].tu2 = tu2Right;
            m_model[index].tv2 = tv2Top;
            index++;

            // Bottom left.
            m_model[index].x  = m_heightMap[index1].x;
            m_model[index].y  = m_heightMap[index1].y;
            m_model[index].z  = m_heightMap[index1].z;
            m_model[index].nx = m_heightMap[index1].nx;
            m_model[index].ny = m_heightMap[index1].ny;
            m_model[index].nz = m_heightMap[index1].nz;
            m_model[index].tu = 0.0f;
            m_model[index].tv = 1.0f;
            m_model[index].tu2 = tu2Left;
            m_model[index].tv2 = tv2Bottom;
            index++;

            // Bottom left.
            m_model[index].x  = m_heightMap[index1].x;
            m_model[index].y  = m_heightMap[index1].y;
            m_model[index].z  = m_heightMap[index1].z;
            m_model[index].nx = m_heightMap[index1].nx;
            m_model[index].ny = m_heightMap[index1].ny;
            m_model[index].nz = m_heightMap[index1].nz;
            m_model[index].tu = 0.0f;
            m_model[index].tv = 1.0f;
            m_model[index].tu2 = tu2Left;
            m_model[index].tv2 = tv2Bottom;
            index++;

            // Upper right.
            m_model[index].x  = m_heightMap[index4].x;
            m_model[index].y  = m_heightMap[index4].y;
            m_model[index].z  = m_heightMap[index4].z;
            m_model[index].nx = m_heightMap[index4].nx;
            m_model[index].ny = m_heightMap[index4].ny;
            m_model[index].nz = m_heightMap[index4].nz;
            m_model[index].tu = 1.0f;
            m_model[index].tv = 0.0f;
            m_model[index].tu2 = tu2Right;
            m_model[index].tv2 = tv2Top;
            index++;

            // Bottom right.
            m_model[index].x  = m_heightMap[index2].x;
            m_model[index].y  = m_heightMap[index2].y;
            m_model[index].z  = m_heightMap[index2].z;
            m_model[index].nx = m_heightMap[index2].nx;
            m_model[index].ny = m_heightMap[index2].ny;
            m_model[index].nz = m_heightMap[index2].nz;
            m_model[index].tu = 1.0f;
            m_model[index].tv = 1.0f;
            m_model[index].tu2 = tu2Right;
            m_model[index].tv2 = tv2Bottom;
            index++;

            // Increment the tu texture coords for the alpha map.
            tu2Left += incrementSize;
            tu2Right += incrementSize;
        }

        // Reset the tu texture coordinate increments for the alpha map.
        tu2Left = 0.0f;
        tu2Right = incrementSize;

        // Increment the tv texture coords for the alpha map.
        tv2Top -= incrementSize;
        tv2Bottom -= incrementSize;
    }

    return true;
}


void TerrainClass::CalculateModelVectors()
{
    int faceCount, i, index;
    TempVertexType vertex1, vertex2, vertex3;
    VectorType tangent, binormal;


    // Calculate the number of faces in the terrain model.
    faceCount = m_vertexCount / 3;

    // Initialize the index to the model data.
    index = 0;

    // Go through all the faces and calculate the the tangent, binormal, and normal vectors.
    for(i=0; i<faceCount; i++)
    {
        // Get the three vertices for this face from the terrain model.
        vertex1.x  = m_model[index].x;
        vertex1.y  = m_model[index].y;
        vertex1.z  = m_model[index].z;
        vertex1.tu = m_model[index].tu;
        vertex1.tv = m_model[index].tv;
        vertex1.nx = m_model[index].nx;
        vertex1.ny = m_model[index].ny;
        vertex1.nz = m_model[index].nz;
        index++;

        vertex2.x  = m_model[index].x;
        vertex2.y  = m_model[index].y;
        vertex2.z  = m_model[index].z;
        vertex2.tu = m_model[index].tu;
        vertex2.tv = m_model[index].tv;
        vertex2.nx = m_model[index].nx;
        vertex2.ny = m_model[index].ny;
        vertex2.nz = m_model[index].nz;
        index++;

        vertex3.x  = m_model[index].x;
        vertex3.y  = m_model[index].y;
        vertex3.z  = m_model[index].z;
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


void TerrainClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, VectorType& tangent,
        VectorType& binormal)
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


bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
    VertexType* vertices;
    unsigned long* indices;
    int i;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;


    // Set the index count to the same as the vertex count.
    m_indexCount = m_vertexCount;

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

    // Load the vertex and index array with data from the terrain model.
    for(i=0; i<m_vertexCount; i++)
    {
        vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
        vertices[i].texture  = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
        vertices[i].normal   = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
        vertices[i].tangent  = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
        vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);
        vertices[i].texture2 = D3DXVECTOR2(m_model[i].tu2, m_model[i].tv2);

        indices[i] = i;
    }

    // Set up the description of the static vertex buffer.
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

    // Now create the vertex buffer.
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

    // Release the arrays now that the buffers have been created and loaded.
    delete [] vertices;
    vertices = 0;

    delete [] indices;
    indices = 0;

    return true;
}


void TerrainClass::ReleaseHeightMap()
{
    if(m_heightMap)
    {
        delete [] m_heightMap;
        m_heightMap = 0;
    }

    return;
}


void TerrainClass::ReleaseModel()
{
    if(m_model)
    {
        delete [] m_model;
        m_model = 0;
    }

    return;
}


void TerrainClass::ReleaseBuffers()
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


void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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
