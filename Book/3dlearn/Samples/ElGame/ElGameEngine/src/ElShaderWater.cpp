#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElWater.h"
#include "ElShaderWater.h"

ElShaderWater::ElShaderWater(ElWater* water)
: mWater(water)
, mCurrentTime(0)
, mStartTime(0)
, mTechnique(NULL)
, mWaveTexture(NULL)
, mEnvTexture(NULL)
, mTime(NULL)
, mViewProj(NULL)
, mWorldViewProj(NULL)
{

}

ElShaderWater::~ElShaderWater()
{

}

void ElShaderWater::update(TimeValue t)
{
	if (mStartTime == 0)
		mStartTime = t;
	mCurrentTime = t;
}

void ElShaderWater::renderImp(ElCamera* camera)
{
	IDirect3DDevice9* d3dDevice = NULL;
	mEffect->GetDevice(&d3dDevice);
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		mEffect->SetTechnique(mTechnique);
		
		TimeValue pastTime = mCurrentTime - mStartTime;
		float time = pastTime * 0.001f;
		mEffect->SetFloat(mTime, time);

		D3DXMATRIX world, view, proj;
		D3DXMatrixIdentity(&world);
		view = camera->getViewMatrix();
		proj = camera->getProjectionMatrix();
		mEffect->SetMatrix(mViewProj, &(view * proj));
		mEffect->SetMatrix(mWorldViewProj, &(world * view * proj));
		
		mEffect->SetTexture(mEnvTexture, mWater->mEnvTex);
		mEffect->SetTexture(mWaveTexture, mWater->mWaveTex.getTexture());

		UINT passes = 0;
		mEffect->Begin(&passes, 0);

		for (UINT pass = 0; pass < passes; ++pass)
		{
			mEffect->BeginPass(pass);

			mWater->drawPrimitive();

			mEffect->EndPass();
		}

		mEffect->End();

		d3dDevice->EndScene();
	}
}

bool ElShaderWater::initialize()
{
	// get & set variables
	mTechnique = mEffect->GetTechniqueByName("WaterTech");
	mWaveTexture = mEffect->GetParameterByName(NULL, "WaveTexture");
	mEnvTexture = mEffect->GetParameterByName(NULL, "ReflectMap");
	mTime = mEffect->GetParameterByName(NULL, "Time");
	mViewProj = mEffect->GetParameterByName(NULL, "ViewProj");
	mWorldViewProj = mEffect->GetParameterByName(NULL, "WorldViewProj");

	return true;
}
