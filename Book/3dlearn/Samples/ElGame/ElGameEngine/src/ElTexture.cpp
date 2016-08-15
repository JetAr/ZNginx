#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElTexture.h"
#include "ElDeviceManager.h"

ElTexture::ElTexture() :
mD3DTexture(NULL)
{
	ZeroMemory(&mSurfaceDesc, sizeof(mSurfaceDesc));
}

ElTexture::~ElTexture()
{
	ElSafeRelease(mD3DTexture);
}

bool ElTexture::loadFromFile(LPCSTR filename)
{
	ElSafeRelease(mD3DTexture);
	if (FAILED(D3DXCreateTextureFromFile(ElDeviceManager::getDevice(), filename, &mD3DTexture)))
		return false;
	
	assert(mD3DTexture->GetLevelCount() > 0);
	mD3DTexture->GetLevelDesc(0, &mSurfaceDesc);

	return true;
}

IDirect3DTexture9* ElTexture::getTexture() const
{
	return mD3DTexture;
}

unsigned int ElTexture::getWidth() const
{
	return mSurfaceDesc.Width;
}

unsigned int ElTexture::getHeight() const
{
	return mSurfaceDesc.Height;
}