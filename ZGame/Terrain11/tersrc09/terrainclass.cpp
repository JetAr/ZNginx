////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"


TerrainClass::TerrainClass()
{
    m_heightMap = 0;
    m_Textures = 0;
    m_Materials = 0;
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}


bool TerrainClass::Initialize(ID3D11Device* device, char* heightMapFilename, char* materialsFilename, char* materialMapFilename, char* colorMapFilename)
{
    bool result;


    // Load in the height map for the terrain.
    result = LoadHeightMap(heightMapFilename);
    if(!result)
    {
        return false;
    }

    // Normalize the height of the height map.
    NormalizeHeightMap();

    // Calculate the normals for the terrain data.
    result = CalculateNormals();
    if(!result)
    {
        return false;
    }

    // Load in the color map for the terrain.
    result = LoadColorMap(colorMapFilename);
    if(!result)
    {
        return false;
    }

    // Initialize the material groups for the terrain.
    result = LoadMaterialFile(materialsFilename, materialMapFilename, device);
    if(!result)
    {
        return false;
    }

    return true;
}


void TerrainClass::Shutdown()
{
    // Release the materials for the terrain.
    ReleaseMaterials();

    // Release the height map data.
    ShutdownHeightMap();

    return;
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

            index = (m_terrainHeight * j) + i;

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


void TerrainClass::NormalizeHeightMap()
{
    int i, j;


    for(j=0; j<m_terrainHeight; j++)
    {
        for(i=0; i<m_terrainWidth; i++)
        {
            m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
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
            index1 = (j * m_terrainHeight) + i;
            index2 = (j * m_terrainHeight) + (i+1);
            index3 = ((j+1) * m_terrainHeight) + i;

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

            index = (j * (m_terrainHeight-1)) + i;

            // Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
            normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
            normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
            normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
        }
    }

    // Now go through all the vertices and take an average of each face normal
    // that the vertex touches to get the averaged normal for that vertex.
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
                index = ((j-1) * (m_terrainHeight-1)) + (i-1);

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Bottom right face.
            if((i < (m_terrainWidth-1)) && ((j-1) >= 0))
            {
                index = ((j-1) * (m_terrainHeight-1)) + i;

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Upper left face.
            if(((i-1) >= 0) && (j < (m_terrainHeight-1)))
            {
                index = (j * (m_terrainHeight-1)) + (i-1);

                sum[0] += normals[index].x;
                sum[1] += normals[index].y;
                sum[2] += normals[index].z;
                count++;
            }

            // Upper right face.
            if((i < (m_terrainWidth-1)) && (j < (m_terrainHeight-1)))
            {
                index = (j * (m_terrainHeight-1)) + i;

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
            index = (j * m_terrainHeight) + i;

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


void TerrainClass::ShutdownHeightMap()
{
    if(m_heightMap)
    {
        delete [] m_heightMap;
        m_heightMap = 0;
    }

    return;
}


bool TerrainClass::LoadColorMap(char* filename)
{
    int error, imageSize, i, j, k, index, colorMapWidth, colorMapHeight;
    FILE* filePtr;
    unsigned int count;
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char* bitmapImage;


    // Open the color map file in binary.
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

    // Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
    colorMapWidth = bitmapInfoHeader.biWidth;
    colorMapHeight = bitmapInfoHeader.biHeight;

    if((colorMapWidth != m_terrainWidth) || (colorMapHeight != m_terrainHeight))
    {
        return false;
    }

    // Calculate the size of the bitmap image data.
    imageSize = colorMapWidth * colorMapHeight * 3;

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

    // Initialize the position in the image data buffer.
    k=0;

    // Read the image data into the color map portion of the height map structure.
    for(j=0; j<colorMapHeight; j++)
    {
        for(i=0; i<colorMapWidth; i++)
        {
            index = (colorMapHeight * j) + i;

            m_heightMap[index].b = (float)bitmapImage[k]   / 255.0f;
            m_heightMap[index].g = (float)bitmapImage[k+1] / 255.0f;
            m_heightMap[index].r = (float)bitmapImage[k+2] / 255.0f;

            k+=3;
        }
    }

    // Release the bitmap image data.
    delete [] bitmapImage;
    bitmapImage = 0;

    return true;
}


bool TerrainClass::LoadMaterialFile(char* filename, char* materialMapFilename, ID3D11Device* device)
{
    ifstream fin;
    char input;
    int i, error;
    char inputFilename[128];
    unsigned int stringLength;
    WCHAR textureFilename[128];
    bool result;


    // Open the materials information text file.
    fin.open(filename);
    if(fin.fail())
    {
        return false;
    }

    // Read up to the value of texture count.
    fin.get(input);
    while(input != ':')
    {
        fin.get(input);
    }

    // Read in the texture count.
    fin >> m_textureCount;

    // Create the texture object array.
    m_Textures = new TextureClass[m_textureCount];
    if(!m_Textures)
    {
        return false;
    }

    // Load each of the textures in.
    for(i=0; i<m_textureCount; i++)
    {
        fin.get(input);
        while(input != ':')
        {
            fin.get(input);
        }

        fin >> inputFilename;

        // Convert the character filename to WCHAR.
        error = mbstowcs_s(&stringLength, textureFilename, 128, inputFilename, 128);
        if(error != 0)
        {
            return false;
        }

        // Load the texture or alpha map.
        result = m_Textures[i].Initialize(device, textureFilename);
        if(!result)
        {
            return false;
        }
    }

    // Read up to the value of the material count.
    fin.get(input);
    while(input != ':')
    {
        fin.get(input);
    }

    // Read in the material count.
    fin >> m_materialCount;

    // Create the material group array.
    m_Materials = new MaterialGroupType[m_materialCount];
    if(!m_Materials)
    {
        return false;
    }

    // Initialize the material group array.
    for(i=0; i<m_materialCount; i++)
    {
        m_Materials[i].vertexBuffer = 0;
        m_Materials[i].indexBuffer = 0;
        m_Materials[i].vertices = 0;
        m_Materials[i].indices = 0;
    }

    // Load each of the material group indexes in.
    for(i=0; i<m_materialCount; i++)
    {
        fin.get(input);
        while(input != ':')
        {
            fin.get(input);
        }

        fin >> m_Materials[i].textureIndex1 >> m_Materials[i].textureIndex2 >> m_Materials[i].alphaIndex;
        fin >> m_Materials[i].red >> m_Materials[i].green >> m_Materials[i].blue;
    }

    // Close the materials information text file.
    fin.close();

    // Now load the material index map.
    result = LoadMaterialMap(materialMapFilename);
    if(!result)
    {
        return false;
    }

    // Load the vertex buffer for each material group with the terrain data.
    result = LoadMaterialBuffers(device);
    if(!result)
    {
        return false;
    }

    return true;
}


bool TerrainClass::LoadMaterialMap(char* filename)
{
    int error, imageSize, i, j, k, index;
    FILE* filePtr;
    unsigned int count;
    BITMAPFILEHEADER bitmapFileHeader;
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char* bitmapImage;


    // Open the material map file in binary.
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

    // Make sure the material index map dimensions are the same as the terrain dimensions for 1 to 1 mapping.
    if((bitmapInfoHeader.biWidth != m_terrainWidth) || (bitmapInfoHeader.biHeight != m_terrainHeight))
    {
        return false;
    }

    // Calculate the size of the bitmap image data.
    imageSize = bitmapInfoHeader.biWidth * bitmapInfoHeader.biHeight * 3;

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

    // Initialize the position in the image data buffer so each vertice has an material index associated with it.
    k=0;

    // Read the material index data into the height map structure.
    for(j=0; j<m_terrainHeight; j++)
    {
        for(i=0; i<m_terrainWidth; i++)
        {
            index = (m_terrainHeight * j) + i;

            m_heightMap[index].rIndex = (int)bitmapImage[k+2];
            m_heightMap[index].gIndex = (int)bitmapImage[k+1];
            m_heightMap[index].bIndex = (int)bitmapImage[k];

            k+=3;
        }
    }

    // Release the bitmap image data.
    delete [] bitmapImage;
    bitmapImage = 0;

    return true;
}


bool TerrainClass::LoadMaterialBuffers(ID3D11Device* device)
{
    int maxVertexCount, maxIndexCount, i, j, index1, index2, index3, index4, redIndex, greenIndex, blueIndex, index, vIndex;
    bool found;
    D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;


    // Create the value for the maximum number of vertices a material group could possibly have.
    maxVertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

    // Set the index count to the same as the maximum vertex count.
    maxIndexCount = maxVertexCount;

    // Initialize vertex and index arrays for each material group to the maximum size.
    for(i=0; i<m_materialCount; i++)
    {
        // Create the temporary vertex array for this material group.
        m_Materials[i].vertices = new VertexType[maxVertexCount];
        if(!m_Materials[i].vertices)
        {
            return false;
        }

        // Create the temporary index array for this material group.
        m_Materials[i].indices = new unsigned long[maxIndexCount];
        if(!m_Materials[i].indices)
        {
            return false;
        }

        // Initialize the counts to zero.
        m_Materials[i].vertexCount = 0;
        m_Materials[i].indexCount = 0;
    }

    // Now loop through the terrain and build the vertex arrays for each material group.
    for(j=0; j<(m_terrainHeight-1); j++)
    {
        for(i=0; i<(m_terrainWidth-1); i++)
        {
            index1 = (m_terrainHeight * j) + i;          // Bottom left.
            index2 = (m_terrainHeight * j) + (i+1);      // Bottom right.
            index3 = (m_terrainHeight * (j+1)) + i;      // Upper left.
            index4 = (m_terrainHeight * (j+1)) + (i+1);  // Upper right.

            // Query the upper left corner vertex for the material index.
            redIndex   = m_heightMap[index3].rIndex;
            greenIndex = m_heightMap[index3].gIndex;
            blueIndex  = m_heightMap[index3].bIndex;

            // Find which material group this vertex belongs to.
            index = 0;
            found = false;
            while(!found)
            {
                if((redIndex == m_Materials[index].red) && (greenIndex == m_Materials[index].green) && (blueIndex == m_Materials[index].blue))
                {
                    found = true;
                }
                else
                {
                    index++;
                }
            }

            // Set the index position in the vertex and index array to the count.
            vIndex = m_Materials[index].vertexCount;

            // Upper left.
            m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
            m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(0.0f, 0.0f);
            m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
            m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index3].r, m_heightMap[index3].g, m_heightMap[index3].b, 1.0f);
            m_Materials[index].indices[vIndex] = vIndex;
            vIndex++;

            // Upper right.
            m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(1.0f, 0.0f);
            m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
            m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
            m_Materials[index].indices[vIndex] = vIndex;
            vIndex++;

            // Bottom left.
            m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(0.0f, 1.0f);
            m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
            m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
            m_Materials[index].indices[vIndex] = vIndex;
            vIndex++;

            // Bottom left.
            m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
            m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(0.0f, 1.0f);
            m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
            m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index1].r, m_heightMap[index1].g, m_heightMap[index1].b, 1.0f);
            m_Materials[index].indices[vIndex] = vIndex;
            vIndex++;

            // Upper right.
            m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
            m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(1.0f, 0.0f);
            m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
            m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index4].r, m_heightMap[index4].g, m_heightMap[index4].b, 1.0f);
            m_Materials[index].indices[vIndex] = vIndex;
            vIndex++;

            // Bottom right.
            m_Materials[index].vertices[vIndex].position = D3DXVECTOR3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
            m_Materials[index].vertices[vIndex].texture = D3DXVECTOR2(1.0f, 1.0f);
            m_Materials[index].vertices[vIndex].normal = D3DXVECTOR3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
            m_Materials[index].vertices[vIndex].color = D3DXVECTOR4(m_heightMap[index2].r, m_heightMap[index2].g, m_heightMap[index2].b, 1.0f);
            m_Materials[index].indices[vIndex] = vIndex;
            vIndex++;

            // Increment the vertex and index array counts.
            m_Materials[index].vertexCount += 6;
            m_Materials[index].indexCount += 6;
        }
    }

    // Now create the vertex and index buffers from the vertex and index arrays for each material group.
    for(i=0; i<m_materialCount; i++)
    {
        vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_Materials[i].vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;

        vertexData.pSysMem = m_Materials[i].vertices;
        vertexData.SysMemPitch = 0;
        vertexData.SysMemSlicePitch = 0;

        result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_Materials[i].vertexBuffer);
        if(FAILED(result))
        {
            return false;
        }

        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_Materials[i].indexCount;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;

        indexData.pSysMem = m_Materials[i].indices;
        indexData.SysMemPitch = 0;
        indexData.SysMemSlicePitch = 0;

        result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_Materials[i].indexBuffer);
        if(FAILED(result))
        {
            return false;
        }

        delete [] m_Materials[i].vertices;
        m_Materials[i].vertices = 0;

        delete [] m_Materials[i].indices;
        m_Materials[i].indices = 0;
    }

    return true;
}


void TerrainClass::ReleaseMaterials()
{
    int i;


    // Release the material groups.
    if(m_Materials)
    {
        for(i=0; i<m_materialCount; i++)
        {
            if(m_Materials[i].vertexBuffer)
            {
                m_Materials[i].vertexBuffer->Release();
                m_Materials[i].vertexBuffer = 0;
            }

            if(m_Materials[i].indexBuffer)
            {
                m_Materials[i].indexBuffer->Release();
                m_Materials[i].indexBuffer = 0;
            }

            if(m_Materials[i].vertices)
            {
                delete [] m_Materials[i].vertices;
                m_Materials[i].vertices = 0;
            }

            if(m_Materials[i].indices)
            {
                delete [] m_Materials[i].indices;
                m_Materials[i].indices = 0;
            }
        }

        delete [] m_Materials;
        m_Materials = 0;
    }

    // Release the terrain textures and alpha maps.
    if(m_Textures)
    {
        for(i=0; i<m_textureCount; i++)
        {
            m_Textures[i].Shutdown();
        }

        delete [] m_Textures;
        m_Textures = 0;
    }

    return;
}


bool TerrainClass::Render(ID3D11DeviceContext* deviceContext, TerrainShaderClass* shader, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
                          D3DXMATRIX projectionMatrix, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor, D3DXVECTOR3 lightDirection)
{
    unsigned int stride;
    unsigned int offset;
    int i;
    bool result;


    // Set vertex buffer stride and offset.
    stride = sizeof(VertexType);
    offset = 0;

    // Set the shader parameters that it will use for rendering.
    result = shader->SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, ambientColor, diffuseColor, lightDirection);
    if(!result)
    {
        return false;
    }

    // Set the type of primitive that should be rendered from the vertex buffers, in this case triangles.
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Render each material group.
    for(i=0; i<m_materialCount; i++)
    {
        // Set the vertex buffer to active in the input assembler so it can be rendered.
        deviceContext->IASetVertexBuffers(0, 1, &m_Materials[i].vertexBuffer, &stride, &offset);

        // Set the index buffer to active in the input assembler so it can be rendered.
        deviceContext->IASetIndexBuffer(m_Materials[i].indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // If the material group has a valid second texture index then this is a blended terrain polygon.
        if(m_Materials[i].textureIndex2 != -1)
        {
            result = shader->SetShaderTextures(deviceContext,  m_Textures[m_Materials[i].textureIndex1].GetTexture(),
                                               m_Textures[m_Materials[i].textureIndex2].GetTexture(),  m_Textures[m_Materials[i].alphaIndex].GetTexture(),
                                               true);
        }

        // If not then it is just a single textured polygon.
        else
        {
            result = shader->SetShaderTextures(deviceContext, m_Textures[m_Materials[i].textureIndex1].GetTexture(), NULL, NULL, false);
        }

        // Check if the textures were set or not.
        if(!result)
        {
            return false;
        }

        // Now render the prepared buffers with the shader.
        shader->RenderShader(deviceContext, m_Materials[i].indexCount);
    }

    return true;
}
