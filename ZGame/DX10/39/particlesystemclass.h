////////////////////////////////////////////////////////////////////////////////
// Filename: particlesystemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PARTICLESYSTEMCLASS_H_
#define _PARTICLESYSTEMCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleSystemClass
////////////////////////////////////////////////////////////////////////////////
class ParticleSystemClass
{
private:
    struct ParticleType
    {
        float positionX, positionY, positionZ;
        float red, green, blue;
        float velocity;
        bool active;
    };

    struct VertexType
    {
        D3DXVECTOR3 position;
        D3DXVECTOR2 texture;
        D3DXVECTOR4 color;
    };

public:
    ParticleSystemClass();
    ParticleSystemClass(const ParticleSystemClass&);
    ~ParticleSystemClass();

    bool Initialize(ID3D10Device*, WCHAR*);
    void Shutdown();
    bool Frame(float);
    void Render(ID3D10Device*);

    ID3D10ShaderResourceView* GetTexture();
    int GetIndexCount();

private:
    bool LoadTexture(ID3D10Device*, WCHAR*);
    void ReleaseTexture();

    bool InitializeParticleSystem();
    void ShutdownParticleSystem();

    bool InitializeBuffers(ID3D10Device*);
    void ShutdownBuffers();

    void EmitParticles(float);
    void UpdateParticles(float);
    void KillParticles();

    bool UpdateBuffers();

    void RenderBuffers(ID3D10Device*);

private:
    float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
    float m_particleVelocity, m_particleVelocityVariation;
    float m_particleSize, m_particlesPerSecond;
    int m_maxParticles;

    int m_currentParticleCount;
    float m_accumulatedTime;

    TextureClass* m_Texture;
    ParticleType* m_particleList;
    int m_vertexCount, m_indexCount;
    VertexType* m_vertices;
    ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
};

#endif
