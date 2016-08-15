#ifndef __ElShaderTerrain_H__
#define __ElShaderTerrain_H__

#include "ElD3DHeaders.h"
#include "ElShader.h"

class ElTerrain;

class ElShaderTerrain : public ElShader
{
public:
	ElShaderTerrain(ElTerrain* terrain);
	~ElShaderTerrain();

protected:
	virtual void renderImp(ElCamera* camera);
	virtual bool initialize();

protected:
	D3DXHANDLE			mTechnique;
	D3DXHANDLE			mWorld;
	D3DXHANDLE			mView;
	D3DXHANDLE			mProjection;
	D3DXHANDLE			mAmbienColor;
	D3DXHANDLE			mDiffuseColor;
	D3DXHANDLE			mDiffuseDirection;
	D3DXHANDLE			mCameraPosition;
	D3DXHANDLE			mTexture[ElTerrainLayers];

	ElTerrain*			mTerrain;
};

#endif //__ElShaderTerrain_H__