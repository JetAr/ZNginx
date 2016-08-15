#ifndef _GUTPARTICLE_
#define _GUTPARTICLE_

#include "Gut.h"
#include "Vector4.h"
#include "FastMath.h"

struct sParticleVertex
{
	float m_fPosition[3];
	unsigned int m_Color;
	float m_fTexcoord[2];
};

struct sPointSpriteVertex
{
	float m_fPosition[3];
	float m_fSize;
	unsigned int m_Color;
};

struct sParticle
{
	Vector4 m_vPosition;
	Vector4 m_vVelocity;
	Vector4 m_vColor;
	float m_fLife;
	float m_fSize;
};

struct sParticleEmitter
{
	// particle emitter
	float m_fEmitRate;
	float m_fEmitTheta;
	float m_fEmitTime;
	// particle initial speed range
	float m_fParticleInitSpeed[2];
	float m_fParticleSize[2];
	// life range for each particle
	float m_fParticleLife[2];

	sParticleEmitter(void)
	{
		m_fEmitRate = 100.0f;
		m_fEmitTheta = FastMath::DegToRad(10.0f);
		m_fEmitTime = 0.0f;
		m_fParticleInitSpeed[0] = 1.0f;
		m_fParticleInitSpeed[1] = 1.0f;
		m_fParticleSize[0] = 0.01f;
		m_fParticleSize[1] = 0.05f;
		m_fParticleLife[0] = 5.0f;
		m_fParticleLife[1] = 5.0f;
	}
};

class CGutParticle
{
public:
	//
	sParticleEmitter m_Emitter;
	//
	int m_iNumMaxParticles;
	int m_iNumParticles;
	float m_fTimeAccmulated;
	float m_fEmitRateScale;
	//
	sParticle *m_pParticleArray;
	sParticleVertex *m_pVertexArray;
	sPointSpriteVertex *m_pPointSpriteArray;
	unsigned short *m_pIndexArray;
	//
	Vector4 m_vForce; // gravity, wind...
	Matrix4x4 m_Matrix;

public:
	CGutParticle(void);
	~CGutParticle();

	void Release(void);
	void SetEmitter(sParticleEmitter &emitter);
	void SetEmitRateScale(float r) { m_fEmitRateScale = r; }
	void SetForce(Vector4 &vForce) { m_vForce = vForce; }

	sParticleEmitter &GetEmitter(void) { return m_Emitter; }

	int EmitParticles(float time_advance);
	int AnimateParticles(float time_advance);
	int Simulate(float time_advance);
	int BuildMesh(Matrix4x4 &view_matrix);
	int BuildPointSprite(void);
};

#endif // _GUTPARTICLE_