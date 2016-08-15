#ifndef __ElWater_H__
#define __ElWater_H__

#include "ElD3DHeaders.h"
#include "ElShaderWater.h"
#include "ElCamera.h"
#include "ElTexture.h"
#include "ElRenderable.h"

class ElShaderWater;

class ElWater : public ElRenderable
{
	friend class ElShaderWater;

public:
	struct VERTEX
	{
		D3DXVECTOR3 pos;
		float u, v;

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};

public:
	ElWater();
	virtual ~ElWater();

	bool create(const D3DXVECTOR3& pos, float width, float length);
	bool beginScene(ElCamera* camera);
	void endScene(ElCamera* camera);
	
	virtual void render(ElCamera* camera, TimeValue t);
	void drawPrimitive();

protected:
	bool createShader();

protected:
	ElShaderWater*		mShader;

	ElTexture			mWaveTex;
	
	IDirect3DTexture9*	mEnvTex;
	IDirect3DSurface9*	mEnvSurface;
	IDirect3DSurface9*	mOldSurface;

	D3DXPLANE			mPlane;
	D3DXVECTOR3			mPos;
	float				mWidth;
	float				mLength;

	VERTEX				mVertices[4];
};

#endif //__ElWater_H__