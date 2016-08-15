#ifndef __ElTexture_H__
#define __ElTexture_H__

#include "ElD3DHeaders.h"

class ElTexture
{
public:
	ElTexture();
	virtual ~ElTexture();

	bool loadFromFile(LPCSTR filename);
	IDirect3DTexture9* getTexture() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;

protected:
	IDirect3DTexture9*	mD3DTexture;
	D3DSURFACE_DESC		mSurfaceDesc;
};

#endif //__ElTexture_H__