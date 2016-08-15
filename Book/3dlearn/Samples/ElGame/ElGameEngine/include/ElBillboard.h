#ifndef __ElBillboard_H__
#define __ElBillboard_H__

#include "ElMovableObject.h"
#include "ElRenderable.h"
#include "ElArray.h"
#include "ElAxisAlignedBox.h"

class ElBillboard : public ElMovableObject, public ElRenderable
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
	ElBillboard();
	virtual ~ElBillboard();

	void setRectangle(float left, float top, float width, float height);
	void setColor(const D3DXCOLOR& top_left, const D3DXCOLOR& top_right, const D3DXCOLOR& bottom_left, const D3DXCOLOR& bottom_right);
	void setTextures(float left, float top, float right, float bottom);
	void setTexture(IDirect3DTexture9* texture);
	void setAlpha(float alpha);
	void setFixedYawAxis(bool fixed);

	void needupdate();

	virtual void update();
	virtual void render(ElCamera* camera, TimeValue t);

	virtual void _updateRenderQueue(ElRenderQueue* queue, ElCamera* cam);
	virtual float getSquaredViewDepth(ElCamera* cam) const;

	virtual const ElAxisAlignedBox& getBoundingBox();

protected:
	void _setTransform(ElCamera* camera);

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

	bool				mYawFixed;

	ElAxisAlignedBox	mAABB;
};

#endif //__ElBillboard_H__