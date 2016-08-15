#include <math.h>
#include "Gut.h"
#include "GutParticle.h"

inline float rand(float min, float max)
{
	int rnd = rand()%1024;
	float r = float(rnd)/1024.0f;

	return min + (max-min) *r;
}

CGutParticle::CGutParticle(void)
{
	m_iNumMaxParticles = 0;
	m_iNumParticles = 0;
	m_vForce.Set(0.0f);
	m_fTimeAccmulated = 0;
	m_fEmitRateScale = 1.0f;

	m_Matrix.Identity();

	m_pVertexArray = NULL;
	m_pIndexArray = NULL;

	m_pParticleArray = NULL;
}

CGutParticle::~CGutParticle()
{
	Release();
}

void CGutParticle::Release(void)
{
	RELEASE_ARRAY(m_pParticleArray);
	RELEASE_ARRAY(m_pVertexArray);
	RELEASE_ARRAY(m_pIndexArray);
}

void CGutParticle::SetEmitter(sParticleEmitter &emitter)
{
	m_Emitter = emitter;

	float particlelife = max(m_Emitter.m_fParticleLife[0], m_Emitter.m_fParticleLife[1]);
	float max_particles = ceil(particlelife * m_Emitter.m_fEmitRate);

	m_iNumMaxParticles = (int) max_particles;
	m_iNumParticles = 0;

	m_pParticleArray = new sParticle[m_iNumMaxParticles];
	m_pPointSpriteArray = new sPointSpriteVertex[m_iNumMaxParticles];

	m_pVertexArray = new sParticleVertex[m_iNumMaxParticles*4];
	m_pIndexArray = new unsigned short[m_iNumMaxParticles*6];

	for ( int i=0; i<m_iNumMaxParticles; i++ )
	{
		int vindex = i*4;
		int iindex = i*6;

		// v0
		m_pVertexArray[vindex].m_fTexcoord[0] = 0.0f;
		m_pVertexArray[vindex].m_fTexcoord[1] = 0.0f;
		// v1
		m_pVertexArray[vindex+1].m_fTexcoord[0] = 0.0f;
		m_pVertexArray[vindex+1].m_fTexcoord[1] = 1.0f;
		// v2
		m_pVertexArray[vindex+2].m_fTexcoord[0] = 1.0f;
		m_pVertexArray[vindex+2].m_fTexcoord[1] = 0.0f;
		// v3
		m_pVertexArray[vindex+3].m_fTexcoord[0] = 1.0f;
		m_pVertexArray[vindex+3].m_fTexcoord[1] = 1.0f;

		// face0
		m_pIndexArray[iindex  ] = vindex;
		m_pIndexArray[iindex+1] = vindex+1;
		m_pIndexArray[iindex+2] = vindex+2;
		// face1
		m_pIndexArray[iindex+3] = vindex+2;
		m_pIndexArray[iindex+4] = vindex+1;
		m_pIndexArray[iindex+5] = vindex+3;
	}
}

int CGutParticle::Simulate(float time_advance)
{
	// 發射新粒子
	EmitParticles(time_advance);
	// 粒子運動模擬
	AnimateParticles(time_advance);
	return m_iNumParticles;
}

// 發射新粒子
int CGutParticle::EmitParticles(float time_advance)
{
	m_fTimeAccmulated += time_advance;
	float fnum_particles = m_fTimeAccmulated * m_Emitter.m_fEmitRate * m_fEmitRateScale;
	int num_particles = (int) fnum_particles;
	if ( num_particles==0 )
		return 0;

	if ( m_iNumParticles + num_particles > m_iNumMaxParticles )
	{
		num_particles = m_iNumMaxParticles - m_iNumParticles;
	}

	Vector4 vDir = m_Matrix[2];
	Vector4 vPos = m_Matrix[3];

	m_fTimeAccmulated = 0;
	sParticle *particle = m_pParticleArray + m_iNumParticles;

	for ( int i=0; i<num_particles; i++, particle++ )
	{
		float speed = rand(m_Emitter.m_fParticleInitSpeed[0], m_Emitter.m_fParticleInitSpeed[1]);
		if ( m_Emitter.m_fEmitTheta )
		{
			float r0 = rand(0.0f, m_Emitter.m_fEmitTheta);
			float r1 = rand(0.0f, MATH_PI*2.0f);
			Matrix4x4 r0_matrix; r0_matrix.RotateX_Replace(r0);
			Matrix4x4 r1_matrix; r1_matrix.RotateZ_Replace(r1);
			Matrix4x4 emit_matrix = r0_matrix * r1_matrix * m_Matrix;
			vDir = emit_matrix[2];
		}
		particle->m_vVelocity = vDir * speed;
		particle->m_vPosition = vPos;
		particle->m_fLife = rand(m_Emitter.m_fParticleLife[0], m_Emitter.m_fParticleLife[1]);
		particle->m_fSize = rand(m_Emitter.m_fParticleSize[0], m_Emitter.m_fParticleSize[1]);
	}

	m_iNumParticles += num_particles;
	return num_particles;
}
// 粒子運動模擬
int CGutParticle::AnimateParticles(float time_advance)
{
	sParticle *particle = m_pParticleArray;
	Vector4 vForce = m_vForce * time_advance;

	for ( int i=0; i<m_iNumParticles; i++ )
	{
		particle->m_fLife -= time_advance;

		if ( particle->m_fLife < 0 )
		{
			// 移除這個粒子
			*particle = m_pParticleArray[--m_iNumParticles];
		}
		else
		{
			// 粒子運動
			particle->m_vPosition += particle->m_vVelocity * time_advance;
			particle->m_vVelocity += vForce;
			particle++;
		}
	}

	return m_iNumParticles;
}
// 建立模型
int CGutParticle::BuildMesh(Matrix4x4 &camera_matrix)
{
	Vector4 vHalf(0.5f);

	for ( int i=0; i<m_iNumParticles; i++ )
	{
		int base = i*4;

		float size = m_pParticleArray[i].m_fSize;
		Vector4 vPos = m_pParticleArray[i].m_vPosition;
		Vector4 vVec0 = camera_matrix[0] * size;
		Vector4 vVec1 = camera_matrix[1] * size;

		Vector4 v0 = vPos - vVec0 * vHalf + vVec1 * vHalf;
		Vector4 v1 = v0 - vVec1;
		Vector4 v2 = v0 + vVec0;
		Vector4 v3 = v0 + vVec0 - vVec1;

		sParticleVertex *p = m_pVertexArray + base;
		float fFadeout = m_pParticleArray[i].m_fLife * 5;
		if ( fFadeout > 1.0f ) fFadeout = 1.0f;

		int intensity = fFadeout * 255;
		unsigned int color = intensity<<24 | intensity<<16 | intensity<<8 | intensity;

		p[0].m_Color = color;
		p[1].m_Color = color;
		p[2].m_Color = color;
		p[3].m_Color = color;

		v0.StoreXYZ(p[0].m_fPosition);
		v1.StoreXYZ(p[1].m_fPosition);
		v2.StoreXYZ(p[2].m_fPosition);
		v3.StoreXYZ(p[3].m_fPosition);
	}

	return m_iNumParticles * 4;
}

int CGutParticle::BuildPointSprite(void)
{
	for ( int i=0; i<m_iNumParticles; i++ )
	{
		sPointSpriteVertex *p = m_pPointSpriteArray + i;
		sParticle *particle = m_pParticleArray + i;
		// 淡出壽命快結束的Particle
		float fFadeout = m_pParticleArray[i].m_fLife * 5;
		if ( fFadeout > 1.0f ) fFadeout = 1.0f;

		int intensity = fFadeout * 255;
		unsigned int color = intensity<<24 | intensity<<16 | intensity<<8 | intensity;

		// 設定位置跟顏色
		particle->m_vPosition.StoreXYZ(p->m_fPosition);
		p->m_fSize = particle->m_fSize;
		p->m_Color = color;
	}

	return m_iNumParticles;
}