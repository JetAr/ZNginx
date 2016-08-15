#include "ElSamplePCH.h"
#include "GUITexture.h"
#include "GUIRenderer.h"
#include "ElDeviceManager.h"

ElGUITexture::ElGUITexture()
: mTexture(NULL)
{
	ZeroMemory(&mSurfaceDesc, sizeof(mSurfaceDesc));
}

ElGUITexture::~ElGUITexture()
{
	ElSafeRelease(mTexture);
}

void ElGUITexture::loadFromFile(const std::string& filename)
{
	ElSafeRelease(mTexture);
	if (FAILED(D3DXCreateTextureFromFile(ElDeviceManager::getDevice(), filename.c_str(), &mTexture)))
		return;

	assert(mTexture->GetLevelCount() > 0);
	mTexture->GetLevelDesc(0, &mSurfaceDesc);
}

void ElGUITexture::loadFromMemory(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, ELGUI::Texture::PixelFormat pixelFormat)
{
	if (pixelFormat == ELGUI::Texture::PF_RGB)
	{

	}
	else if (pixelFormat == ELGUI::Texture::PF_RGBA)
	{

	}
	else
		assert(0 && "ElGUITexture::loadFromMemory - Unknown pixel Format.");
}

void ElGUITexture::loadAsRenderTarget(unsigned int width, unsigned int height, ELGUI::Texture::PixelFormat pixelFormat)
{
	D3DFORMAT format;
	if (pixelFormat == ELGUI::Texture::PF_RGB)
		format = D3DFMT_R8G8B8;
	else if (pixelFormat == ELGUI::Texture::PF_RGBA)
		format = D3DFMT_A8R8G8B8;
	else
		assert(0 && "ElGUITexture::loadAsRenderTarget - Unknown pixel Format.");

	D3DXCreateTexture(ElDeviceManager::getDevice(), width, height, 1,
		D3DUSAGE_RENDERTARGET, format,
		D3DPOOL_DEFAULT, &mTexture);
	mTexture->GetLevelDesc(0, &mSurfaceDesc);
}

void ElGUITexture::setTexture(IDirect3DTexture9* tex)
{
	mTexture = tex;
	mTexture->GetLevelDesc(0, &mSurfaceDesc);
}

IDirect3DTexture9* ElGUITexture::getTexture() const
{
	return mTexture;
}

unsigned short ElGUITexture::getWidth() const
{
	return mSurfaceDesc.Width;
}

unsigned short ElGUITexture::getHeight() const
{
	return mSurfaceDesc.Height;
}


