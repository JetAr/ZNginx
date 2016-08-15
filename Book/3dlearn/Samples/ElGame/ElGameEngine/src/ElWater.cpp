#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElWater.h"
#include "ElShaderWater.h"
#include "ElSceneManager.h"
#include "ElDeviceManager.h"

#define ElWaterWaveTexFile		"..\\Media\\Water\\Wave.bmp"
#define ElWaterShaderFile		"..\\Media\\Water\\Water.fx"

ElWater::ElWater()
: mShader(NULL)
, mEnvTex(NULL)
, mEnvSurface(NULL)
, mOldSurface(NULL)
{

}

ElWater::~ElWater()
{
	ElSafeDelete(mShader);
	ElSafeRelease(mEnvSurface);
	ElSafeRelease(mEnvTex);
	ElSafeRelease(mOldSurface);
}

bool ElWater::create(const D3DXVECTOR3& pos, float width, float length)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// load wave tex
	mWaveTex.loadFromFile(ElWaterWaveTexFile);

	// create render target
	const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();
	D3DXCreateTexture(d3dDevice, backBufferDesc.Width, backBufferDesc.Height, 1,
					D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT, &mEnvTex);
	mEnvTex->GetSurfaceLevel(0, &mEnvSurface);

	// build water mesh
	mPos = pos;
	mWidth = width;
	mLength = length;

	mVertices[0].pos = mPos;  
	mVertices[0].u = 0.0f;
	mVertices[0].v = 0.0f;

	mVertices[1].pos = D3DXVECTOR3(mPos.x + mLength, mPos.y, mPos.z);
	mVertices[1].u = 1.0f;
	mVertices[1].v = 0.0f;

	mVertices[2].pos = D3DXVECTOR3(mPos.x, mPos.y, mPos.z - mWidth);
	mVertices[2].u = 0.0f;
	mVertices[2].v = 1.0f;

	mVertices[3].pos = D3DXVECTOR3(mPos.x + mLength, mPos.y, mPos.z - mWidth);
	mVertices[3].u = 1.0f;
	mVertices[3].v = 1.0f;

	D3DXVECTOR3 normal(0.0f, 1.0f, 0.0f);
	D3DXPlaneFromPointNormal(&mPlane, &mPos, &normal);
	D3DXPlaneNormalize(&mPlane, &mPlane);

	// build shader
	if (!createShader())
		return false;

	return true;
}

bool ElWater::beginScene(ElCamera* camera)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// get the reflected view matrix, and set it to device
	if (camera)
	{
		camera->enableReflection(mPlane);
		d3dDevice->SetTransform(D3DTS_VIEW, &camera->getViewMatrix());
	}

	// set water's environment map as the render target
	d3dDevice->GetRenderTarget(0, &mOldSurface);
	d3dDevice->SetRenderTarget(0, mEnvSurface);
	
	// set clip plane
	d3dDevice->SetClipPlane(0, mPlane);

	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	d3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(192, 192, 192), 1.0f, 0);

	return true;
}

void ElWater::endScene(ElCamera* camera)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	d3dDevice->SetRenderTarget(0, mOldSurface);
	ElSafeRelease(mOldSurface);
	d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(192, 192, 192), 1.0f, 0);
	d3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
	d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
	if (camera)
	{
		camera->disableReflection();
		d3dDevice->SetTransform(D3DTS_VIEW, &camera->getViewMatrix());
	}
}

void ElWater::render(ElCamera* camera, TimeValue t)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->SetFVF(VERTEX::FVF);
	mShader->update(t);
	mShader->render(camera);
}

void ElWater::drawPrimitive()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertices, sizeof(VERTEX));
}

bool ElWater::createShader()
{
	ElSafeDelete(mShader);
	mShader = ElNew ElShaderWater(this);
	if (!mShader->create(ElWaterShaderFile))
		return false;

	return true;
}

