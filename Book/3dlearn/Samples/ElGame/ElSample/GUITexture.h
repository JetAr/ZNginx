#ifndef __ElGUITexture_H__
#define __ElGUITexture_H__

#include "ELGUI.h"
#include "ElD3DHeaders.h"

class ElGUITexture : public ELGUI::Texture
{
public:
	ElGUITexture();
	virtual ~ElGUITexture();

	virtual void loadFromFile(const std::string& filename);
	virtual void loadFromMemory(const void* buffPtr, unsigned int buffWidth, unsigned int buffHeight, ELGUI::Texture::PixelFormat pixelFormat);
	virtual void loadAsRenderTarget(unsigned int width, unsigned int height, ELGUI::Texture::PixelFormat pixelFormat);

	virtual void setTexture(IDirect3DTexture9* tex);
	virtual IDirect3DTexture9* getTexture() const;
	virtual	unsigned short getWidth() const;
	virtual	unsigned short getHeight() const;

protected:
	IDirect3DTexture9*	mTexture;
	D3DSURFACE_DESC		mSurfaceDesc;
};

#endif //__ElGUITexture_H__