////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "particlesystemclass.h"


ParticleSystemClass::ParticleSystemClass()
{
    m_Texture = 0;
    m_particleList = 0;
    m_vertices = 0;
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
}


ParticleSystemClass::ParticleSystemClass(const ParticleSystemClass& other)
{
}


ParticleSystemClass::~ParticleSystemClass()
{
}


bool ParticleSystemClass::Initialize(ID3D10Device* device, WCHAR* textureFilename)
{
    bool result;


    // Load the texture that is used for the particles.
    result = LoadTexture(device, textureFilename);
    if(!result)
    {
        return false;
    }

    // Initialize the particle system.
    result = InitializeParticleSystem();
    if(!result)
    {
        return false;
    }

    // Create the buffers that will be used to render the particles with.
    result = InitializeBuffers(device);
    if(!result)
    {
        return false;
    }

    return true;
}


void ParticleSystemClass::Shutdown()
{
    // Release the buffers.
    ShutdownBuffers();

    // Release the particle system.
    ShutdownParticleSystem();

    // Release the texture used for the particles.
    ReleaseTexture();

    return;
}


bool ParticleSystemClass::Frame(float frameTime)
{
    bool result;


    // Release old particles.
    KillParticles();

    // Emit new particles.
    EmitParticles(frameTime);

    // Update the position of the particles.
    UpdateParticles(frameTime);

    // Update the dynamic vertex buffer with the new position of each particle.
    result = UpdateBuffers();
    if(!result)
    {
        return false;
    }

    return true;
}


void ParticleSystemClass::Render(ID3D10Device* device)
{
    // Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
    RenderBuffers(device);

    return;
}


ID3D10ShaderResourceView* ParticleSystemClass::GetTexture()
{
    return m_Texture->GetTexture();
}


int ParticleSystemClass::GetIndexCount()
{
    return m_indexCount;
}


bool ParticleSystemClass::LoadTexture(ID3D10Device* device, WCHAR* filename)
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


void ParticleSystemClass::ReleaseTexture()
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


bool ParticleSystemClass::InitializeParticleSystem()
{
    int i;


    // Set the random deviation of where the particles can be located when emitted.
    m_particleDeviationX = 0.5f;
    m_particleDeviationY = 0.1f;
    m_particleDeviationZ = 2.0f;

    // Set the speed and speed variation of particles.
    m_particleVelocity = 1.0f;
    m_particleVelocityVariation = 0.2f;

    // Set the physical size of the particles.
    m_particleSize = 0.2f;

    // Set the number of particles to emit per second.
    m_particlesPerSecond = 250.0f;

    // Set the maximum number of particles allowed in the particle system.
    m_maxParticles = 5000;

    // Create the particle list.
    m_particleList = new ParticleType[m_maxParticles];
    if(!m_particleList)
    {
        return false;
    }

    // Initialize the particle list.
    for(i=0; i<m_maxParticles; i++)
    {
        m_particleList[i].active = false;
    }

    // Initialize the current particle count to zero since none are emitted yet.
    m_currentParticleCount = 0;

    // Clear the initial accumulated time for the particle per second emission rate.
    m_accumulatedTime = 0.0f;

    return true;
}


void ParticleSystemClass::ShutdownParticleSystem()
{
    // Release the particle list.
    if(m_particleList)
    {
        delete [] m_particleList;
        m_particleList = 0;
    }

    return;
}


bool ParticleSystemClass::InitializeBuffers(ID3D10Device* device)
{
    unsigned long* indices;
    int i;
    D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D10_SUBRESOURCE_DATA vertexData, indexData;
    HRESULT result;


    // Set the maximum number of vertices in the vertex array.
    m_vertexCount = m_maxParticles * 6;

    // Set the maximum number of indices in the index array.
    m_indexCount = m_vertexCount;

    // Create the vertex array for the particles that will be rendered.
    m_vertices = new VertexType[m_vertexCount];
    if(!m_vertices)
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
    memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

    // Initialize the index array.
    for(i=0; i<m_indexCount; i++)
    {
        indices[i] = i;
    }

    // Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;

    // Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;

    // Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if(FAILED(result))
    {
        return false;
    }

    // Set up the description of the static index buffer.
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

    // Release the index array since it is no longer needed.
    delete [] indices;
    indices = 0;

    return true;
}


void ParticleSystemClass::ShutdownBuffers()
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


void ParticleSystemClass::EmitParticles(float frameTime)
{
    bool emitParticle, found;
    float positionX, positionY, positionZ, velocity, red, green, blue;
    int index, i, j;


    // Increment the frame time.
    m_accumulatedTime += frameTime;

    // Set emit particle to false for now.
    emitParticle = false;

    // Check if it is time to emit a new particle or not.
    if(m_accumulatedTime > (1000.0f / m_particlesPerSecond))
    {
        m_accumulatedTime = 0.0f;
        emitParticle = true;
    }

    // If there are particles to emit then emit one per frame.
    if((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
    {
        m_currentParticleCount++;

        // Now generate the randomized particle properties.
        positionX = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationX;
        positionY = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationY;
        positionZ = (((float)rand()-(float)rand())/RAND_MAX) * m_particleDeviationZ;

        velocity = m_particleVelocity + (((float)rand()-(float)rand())/RAND_MAX) * m_particleVelocityVariation;

        red   = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
        green = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;
        blue  = (((float)rand()-(float)rand())/RAND_MAX) + 0.5f;

        // Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
        // We will sort using Z depth so we need to find where in the list the particle should be inserted.
        index = 0;
        found = false;
        while(!found)
        {
            if((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
            {
                found = true;
            }
            else
            {
                index++;
            }
        }

        // Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
        i = m_currentParticleCount;
        j = i - 1;

        while(i != index)
        {
            m_particleList[i].positionX = m_particleList[j].positionX;
            m_particleList[i].positionY = m_particleList[j].positionY;
            m_particleList[i].positionZ = m_particleList[j].positionZ;
            m_particleList[i].red       = m_particleList[j].red;
            m_particleList[i].green     = m_particleList[j].green;
            m_particleList[i].blue      = m_particleList[j].blue;
            m_particleList[i].velocity  = m_particleList[j].velocity;
            m_particleList[i].active    = m_particleList[j].active;
            i--;
            j--;
        }

        // Now insert it into the particle array in the correct depth order.
        m_particleList[index].positionX = positionX;
        m_particleList[index].positionY = positionY;
        m_particleList[index].positionZ = positionZ;
        m_particleList[index].red       = red;
        m_particleList[index].green     = green;
        m_particleList[index].blue      = blue;
        m_particleList[index].velocity  = velocity;
        m_particleList[index].active    = true;
    }

    return;
}


void ParticleSystemClass::UpdateParticles(float frameTime)
{
    int i;


    // Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
    for(i=0; i<m_currentParticleCount; i++)
    {
        m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
    }

    return;
}


void ParticleSystemClass::KillParticles()
{
    int i, j;


    // Kill all the particles that have gone below a certain height range.
    for(i=0; i<m_maxParticles; i++)
    {
        if((m_particleList[i].active == true) && (m_particleList[i].positionY < -3.0f))
        {
            m_particleList[i].active = false;
            m_currentParticleCount--;

            // Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
            for(j=i; j<m_maxParticles-1; j++)
            {
                m_particleList[j].positionX = m_particleList[j+1].positionX;
                m_particleList[j].positionY = m_particleList[j+1].positionY;
                m_particleList[j].positionZ = m_particleList[j+1].positionZ;
                m_particleList[j].red       = m_particleList[j+1].red;
                m_particleList[j].green     = m_particleList[j+1].green;
                m_particleList[j].blue      = m_particleList[j+1].blue;
                m_particleList[j].velocity  = m_particleList[j+1].velocity;
                m_particleList[j].active    = m_particleList[j+1].active;
            }
        }
    }

    return;
}


bool ParticleSystemClass::UpdateBuffers()
{
    void* verticesPtr;
    HRESULT result;
    int index, i;


    // Initialize vertex array to zeros at first.
    memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

    // Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
    index = 0;

    for(i=0; i<m_currentParticleCount; i++)
    {
        // Bottom left.
        m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
        m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
        m_vertices[index
                  ].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
        index++;

        // Top left.
        m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
        m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
        m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
        index++;

        // Bottom right.
        m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
        m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
        m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
        index++;

        // Bottom right.
        m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
        m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
        m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
        index++;

        // Top left.
        m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
        m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
        m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
        index++;

        // Top right.
        m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
        m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
        m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
        index++;
    }

    // Initialize the vertex buffer pointer to null first.
    verticesPtr = 0;

    // Lock the vertex buffer.
    result = m_vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&verticesPtr);
    if(FAILED(result))
    {
        return false;
    }

    // Copy the vertex array data into the dynamic vertex buffer.
    memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

    // Unlock the vertex buffer.
    m_vertexBuffer->Unmap();

    return true;
}


void ParticleSystemClass::RenderBuffers(ID3D10Device* device)
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

    // Set the type of primitive that should be rendered from this vertex buffer.
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return;
}
