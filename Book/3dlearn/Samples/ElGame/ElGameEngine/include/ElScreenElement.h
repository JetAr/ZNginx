#ifndef __ElScreenElement_H__
#define __ElScreenElement_H__

#include "ElD3DHeaders.h"
#include "ElArray.h"
#include <vector>

class ElScreenElement
{
public:
	struct VERTEX
	{
		D3DXVECTOR3			pos;
		DWORD				color;
		D3DXVECTOR2			tex;

		static const DWORD	FVF =  D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	};

public:
	ElScreenElement();
	virtual ~ElScreenElement();

	void setRectangle(float left, float top, float width, float height);
	void setColor(const D3DXCOLOR& top_left, const D3DXCOLOR& top_right, const D3DXCOLOR& bottom_left, const D3DXCOLOR& bottom_right);
	void setTextures(float left, float top, float right, float bottom);
	void setTexture(IDirect3DTexture9* texture);
	
	void needupdate();

	// get absolute width / height in pixel
	unsigned int getWidth();
	unsigned int getHeight();

	void render();

protected:
	void _update();

protected:
	bool				mNeedUpdate;

	IDirect3DTexture9*	mTexture;
	ElArray<VERTEX>		mVertices;

	// Vertex
	float				mLeft;
	float				mTop;
	float				mWidth;
	float				mHeight;

	// Vertex Diffuse
	ColorValue			mTopLeftColor;
	ColorValue			mTopRightColor;
	ColorValue			mBottomLeftColor;
	ColorValue			mBottomRightColor;

	// Texture coordinate
	float				mTexLeft;
	float				mTexTop;
	float				mTexRight;
	float				mTexBottom;
};

typedef std::vector<ElScreenElement*> ElScreenElementList;
typedef std::vector<ElScreenElement*>::iterator ElScreenElementIterator;
typedef std::vector<ElScreenElement*>::const_iterator ElScreenElementConstIterator;

#endif //__ElScreenElement_H__