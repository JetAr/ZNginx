#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElSkybox.h"
#include "ElSceneManager.h"
#include "ElDeviceManager.h"

ElSkybox::ElSkybox()
: mSizeX(0.0f)
, mSizeY(0.0f)
, mSizeZ(0.0f)
, mVerties(NULL)
{
	memset(mTextures, NULL, sizeof(mTextures));
}

ElSkybox::~ElSkybox()
{
	ElSafeDeleteArray(mVerties);

	for (int i = 0; i < 6; ++i)
	{
		ElSafeDelete(mTextures[i]);
	}
}

bool ElSkybox::create(float sizeX, float sizeY, float sizeZ)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// setup skybox mesh
	mSizeX = sizeX;
	mSizeY = sizeY;
	mSizeZ = sizeZ;

	ElSafeDeleteArray(mVerties);
	mVerties = ElNew VERTEX[36];

	int idx = 0;
	
	// front
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY,  mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY,  mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY, mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY, mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY,  mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY, mSizeZ, 1.0f, 1.0f);
	
	// right
	mVerties[idx++] = VERTEX( mSizeX, mSizeY,  mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY, -mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY, mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY, mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY, -mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY,-mSizeZ, 1.0f, 1.0f);
	
	// back
	mVerties[idx++] = VERTEX( mSizeX, mSizeY, -mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY, -mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY,-mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY,-mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY, -mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY,-mSizeZ, 1.0f, 1.0f);

	// left
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY, -mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY,  mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY,-mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY,-mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY,  mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY, mSizeZ, 1.0f, 1.0f);

	// top
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY, -mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY, -mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY,  mSizeZ, 1.0f, 1.0f);
	mVerties[idx++] = VERTEX(-mSizeX, mSizeY,  mSizeZ, 1.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY, -mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, mSizeY,  mSizeZ, 1.0f, 0.0f);

	// bottom
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY,-mSizeZ, 0.0f, 0.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY, mSizeZ, 1.0f, 0.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY, mSizeZ, 1.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY, mSizeZ, 1.0f, 1.0f);
	mVerties[idx++] = VERTEX( mSizeX, -mSizeY,-mSizeZ, 0.0f, 1.0f);
	mVerties[idx++] = VERTEX(-mSizeX, -mSizeY,-mSizeZ, 0.0f, 0.0f);

	return true;
}

bool ElSkybox::setTexture(SkyboxPlane plane, LPCSTR filename)
{
	assert(plane >= SKYBOX_PLANE_FRONT && plane <= SKYBOX_PLANE_BOTTOM);
	ElTexture* texture = ElNew ElTexture;
	if (!texture->loadFromFile(filename))
		return false;
	mTextures[plane] = texture;
}

void ElSkybox::render(ElCamera* camera, TimeValue t)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		D3DXVECTOR3 cameraPos;
		cameraPos = camera->getRealPosition();
		if (camera->isReflected())
		{
			d3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
			D3DXVec3TransformCoord(&cameraPos, &cameraPos, &camera->getReflectionMatrix());
		}

		D3DXMATRIX matWorld;
		D3DXMatrixTranslation(&matWorld, cameraPos.x, cameraPos.y, cameraPos.z);
		d3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		d3dDevice->SetFVF(VERTEX::FVF);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		for(int i = 0; i < 6; ++i)
		{
			if (mTextures[i]->getTexture())
			{
				d3dDevice->SetTexture(0, mTextures[i]->getTexture());
				d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, mVerties + i * 6, sizeof(VERTEX));
			}
		}
		d3dDevice->EndScene();
		d3dDevice->SetTexture(0, NULL);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		d3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		d3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		if (camera->isReflected())
			d3dDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	}
}
