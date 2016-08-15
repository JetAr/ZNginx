#ifndef __ELNameboard_H__
#define __ELNameboard_H__

#include "ElD3DHeaders.h"
#include "ElDefines.h"
#include <string>

class ElNameboard
{
public:
	ElNameboard();
	virtual ~ElNameboard();

	bool create(std::wstring text, std::string facename, int fontsize, unsigned int fontcolour = 0xFFFFFF);
	
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	IDirect3DTexture9* getTexture() const;

protected:
	bool _createTexture();

protected:
	IDirect3DTexture9*		mTexture;
	unsigned int			mWidth;
	unsigned int			mHeight;
};

#endif //__ELNameboard_H__