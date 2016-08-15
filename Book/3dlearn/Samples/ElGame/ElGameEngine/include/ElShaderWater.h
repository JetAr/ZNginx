#ifndef __ElShader_Water_H__
#define __ElShader_Water_H__

#include "ElD3DHeaders.h"
#include "ElShader.h"

class ElWater;

class ElShaderWater : public ElShader
{
public:
	ElShaderWater(ElWater* water);
	~ElShaderWater();
	
	void update(TimeValue t);

protected:
	virtual void renderImp(ElCamera* camera);
	virtual bool initialize();

protected:
	D3DXHANDLE mTechnique;
	D3DXHANDLE mWaveTexture;
	D3DXHANDLE mEnvTexture;
	D3DXHANDLE mTime;
	D3DXHANDLE mViewProj;
	D3DXHANDLE mWorldViewProj;

	TimeValue mCurrentTime;
	TimeValue mStartTime;

	ElWater* mWater;
};

#endif //__ElShaderWater_H__