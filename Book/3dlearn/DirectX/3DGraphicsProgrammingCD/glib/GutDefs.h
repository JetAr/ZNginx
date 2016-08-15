#ifndef _GUT_DEFS_
#define _GUT_DEFS_

#define SAFE_RELEASE(x) if (x) { x->Release(); x=NULL; }
#define RELEASE_ARRAY(x) if ( x ) { delete [] x; x=NULL; }

#include "Vector4.h"

enum GutEnum
{
	GUT_UNKNOWN = 1,
	// device
	GUT_OPENGL,
	GUT_DX9,
	GUT_DX10,
};

struct Vertex_V3T2
{
	float m_Position[3]; // 頂點位置
	float m_Texcoord[2]; // 貼圖座標
};

struct Vertex_V3
{
	float m_Position[3];
};

struct Vertex_V3N3
{
	float m_Position[3];
	float m_Normal[3];
};

struct Vertex_V3N3C
{
	float m_Position[3];
	float m_Normal[3];
	unsigned char m_Color[4];
};

struct Vertex_V4T4
{
	Vector4 m_vPosition; // 頂點位置
	Vector4 m_vTexcoord; // 貼圖座標
};

struct GutCallBack
{
	void (*OnSize)(int x, int y);
	void (*OnPaint)(void);
	void (*OnIdle)(void);
	void (*OnClose)(void);

	GutCallBack(void);
};

struct GutDeviceSpec
{
	int m_iX, m_iY; // position
	int m_iWidth, m_iHeight; // size
	int m_iMultiSamples;
	bool m_bFullScreen; // fullscreen or windowed
	char *m_szDevice;

	GutDeviceSpec(void)
	{
		m_iX = m_iY = 256;
		m_iWidth = m_iHeight = 512;
		m_iMultiSamples = 0;
		m_bFullScreen = false;
		m_szDevice = NULL;
	}
};

#define LIGHT_POINT			0
#define LIGHT_DIRECTIONAL	1
#define LIGHT_SPOT			2
#define LIGHT_MAX_LIGHTS	8

struct sGutLight
{
	// position & orientation
	Vector4 m_vPosition;
	Vector4 m_vDirection;
	// color & intensity
	Vector4 m_vAmbient;
	Vector4 m_vDiffuse;
	Vector4 m_vSpecular;
	// attenuation
	Vector4 m_vAttenuation;
	// spotlight effect
	float  m_fSpotlightInnerCone;
	float  m_fSpotlightCutoffCosine;
	float  m_fSpotlightExponent;	
	float  m_fPadding;
	// type & enable/disable
	int	   m_iLightType;
	int	   m_bEnabled;
	int	   m_padding[2];

	sGutLight(void)
	{
		m_vPosition.Set(0.0f, 0.0f, 0.0f, 1.0f);
		m_vDirection.Set(0.0f, 0.0f, 1.0f, 0.0f);

		m_vAmbient.Set(1.0f);
		m_vDiffuse.Set(1.0f);
		m_vSpecular.Set(0.0f);

		m_vAttenuation.Set(1.0f, 0.0f, 0.0f);

		m_iLightType = LIGHT_POINT;
		m_bEnabled = false;
	}
};

// for fixed pipeline material
#define MAX_NUM_TEXTURES 3
#define MAX_TEXCOORDS 3

// auto gen map channel ID
#define MAP_SPHEREMAP 32
#define MAP_CUBEMAP   33
#define MAP_NOOVERWRITE 0xff

struct sVertexDecl
{
	// position
	int m_iPositionOffset;
	int m_iNumPositionElements;
	// normal
	int m_iNormalOffset;
	int m_iNumNormalElements;
	// color
	int m_iColorOffset;
	int m_iNumColorElements;
	// texcoord
	int m_iTexcoordOffset[MAX_TEXCOORDS];
	int m_iNumTexcoordElements[MAX_TEXCOORDS];
	// tangent
	int m_iTangentOffset;
	int m_iNumTangentElements;
	// binormal
	int m_iBiNormalOffset;
	int m_iBiNormalElements;
	// stride
	int m_iVertexSize;

	sVertexDecl(void)
	{
		m_iPositionOffset = 0;
		m_iNumPositionElements = 3;
		m_iNormalOffset = -1;
		m_iNumNormalElements = 0;
		m_iColorOffset = -1;
		m_iNumColorElements = 0;
		for ( int i=0; i<MAX_TEXCOORDS; i++ )
		{
			m_iTexcoordOffset[i] = -1;
			m_iNumTexcoordElements[i] = 0;
		}
		m_iTangentOffset = -1;
		m_iNumTangentElements = 0;
		m_iBiNormalOffset = -1;
		m_iBiNormalElements = 0;
		m_iVertexSize = 0;
	}

	void SetVertexFormat(int PosElems, int NormalElems, int ColorElems, 
		int Tex0Elems=0, int Tex1Elems=0, int Tex2Elems=0, int Tex3Elems=0);
};

#endif // _GUTIL_