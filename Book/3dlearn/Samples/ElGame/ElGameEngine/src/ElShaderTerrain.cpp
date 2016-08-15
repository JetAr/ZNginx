#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElTerrain.h"
#include "ElShaderTerrain.h"

ElShaderTerrain::ElShaderTerrain(ElTerrain* terrain)
: mTechnique(NULL)
, mWorld(NULL)
, mView(NULL)
, mProjection(NULL)
, mAmbienColor(NULL)
, mDiffuseColor(NULL)
, mDiffuseDirection(NULL)
, mCameraPosition(NULL)
, mTerrain(terrain)
{
	memset((void*)mTexture, NULL, sizeof(mTexture));
}

ElShaderTerrain::~ElShaderTerrain()
{

}

bool ElShaderTerrain::initialize()
{
	// Get D3DXHANDLEs to the parameters/techniques that are set every frame so 
	// D3DX doesn't spend time doing string compares. Doing this likely won't affect
	// the perf of simple sampled but it should be done in complex engine.
	mTechnique = mEffect->GetTechniqueByName("TerrainBlend");
	
	mWorld = mEffect->GetParameterByName(NULL, "World");
	mView = mEffect->GetParameterByName(NULL, "View");
	mProjection = mEffect->GetParameterByName(NULL, "Projection");
	mAmbienColor = mEffect->GetParameterByName(NULL, "AmbientColor");
	mDiffuseColor = mEffect->GetParameterByName(NULL, "DiffuseColor");
	mDiffuseDirection = mEffect->GetParameterByName(NULL, "DiffuseLightDirection");
	mCameraPosition = mEffect->GetParameterByName(NULL, "CameraPosition");

	mTexture[0] = mEffect->GetParameterByName(NULL, "texture0");
	mTexture[1] = mEffect->GetParameterByName(NULL, "texture1");
	mTexture[2] = mEffect->GetParameterByName(NULL, "texture2");
	mTexture[3] = mEffect->GetParameterByName(NULL, "texture3");

	// Check if the current technique is valid for the ElNew device/settings
	if (FAILED(mEffect->ValidateTechnique(mTechnique)))
		return false;

	return true;
}

void ElShaderTerrain::renderImp(ElCamera* camera)
{
	IDirect3DDevice9* d3dDevice = NULL;
	mEffect->GetDevice(&d3dDevice);
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		// set shader variables
		mEffect->SetTechnique(mTechnique);

		mEffect->SetTexture(mTexture[0], mTerrain->mTexture[0].getTexture());
		mEffect->SetTexture(mTexture[1], mTerrain->mTexture[1].getTexture());
		mEffect->SetTexture(mTexture[2], mTerrain->mTexture[2].getTexture());
		mEffect->SetTexture(mTexture[3], mTerrain->mTexture[3].getTexture());

		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		D3DXMatrixTranslation(&world, 0.0f, 0.0f, 0.0f);
		D3DXMATRIX view = camera->getViewMatrix();
		D3DXMATRIX projection = camera->getProjectionMatrix();
		mEffect->SetMatrix(mWorld, &world);
		mEffect->SetMatrix(mView, &view);
		mEffect->SetMatrix(mProjection, &projection);
		D3DXMATRIX worldInverseTranspose;
		D3DXMatrixInverse(&worldInverseTranspose, NULL, &worldInverseTranspose);
		D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);

		mEffect->SetVector(mAmbienColor, &D3DXVECTOR4(D3DXCOLOR(mTerrain->mAmbient)));
		if (mTerrain->mDiffuse)
		{
			mEffect->SetVector(mDiffuseColor, &D3DXVECTOR4(D3DXCOLOR(mTerrain->mDiffuse->Diffuse)));
			mEffect->SetVector(mDiffuseDirection, &D3DXVECTOR4(mTerrain->mDiffuse->Direction, 1.0f));
		}

		mEffect->SetVector(mCameraPosition, &D3DXVECTOR4(camera->getRealPosition(), 1.0f));

		// render
		UINT passes = 0;
		mEffect->Begin(&passes, 0);
		for (UINT pass = 0; pass < passes; ++pass)
		{
			mEffect->BeginPass(pass);
			
			mTerrain->drawPrimitive();
			
			mEffect->EndPass();
		}
		mEffect->End();

		d3dDevice->EndScene();
	}
}

