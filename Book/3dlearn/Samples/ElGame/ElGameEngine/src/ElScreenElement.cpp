#include "ElApplicationPCH.h"
#include "ElScreenElement.h"
#include "ElDeviceManager.h"

ElScreenElement::ElScreenElement()
: mNeedUpdate(true)
, mTexture(NULL)
, mLeft(0.0f)
, mTop(0.0f)
, mWidth(0.0f)
, mHeight(0.0f)
, mTopLeftColor(D3DCOLOR_ARGB(255, 255, 255, 255))
, mTopRightColor(D3DCOLOR_ARGB(255, 255, 255, 255))
, mBottomLeftColor(D3DCOLOR_ARGB(255, 255, 255, 255))
, mBottomRightColor(D3DCOLOR_ARGB(255, 255, 255, 255))
, mTexLeft(0.0f)
, mTexTop(0.0f)
, mTexRight(0.0f)
, mTexBottom(0.0f)
{
	mVertices.assign(4);
	needupdate();
}

ElScreenElement::~ElScreenElement()
{

}

void ElScreenElement::setRectangle(float left, float top, float width, float height)
{
	mLeft = left;
	mTop = top;
	mWidth = width;
	mHeight = height;
	needupdate();
}

void ElScreenElement::setColor(const D3DXCOLOR& top_left, const D3DXCOLOR& top_right, const D3DXCOLOR& bottom_left, const D3DXCOLOR& bottom_right)
{
	mTopLeftColor = top_left;
	mTopRightColor = top_right;
	mBottomLeftColor = bottom_left;
	mBottomRightColor = bottom_right;
	needupdate();
}

void ElScreenElement::setTextures(float left, float top, float right, float bottom)
{
	mTexLeft = left;
	mTexTop = top;
	mTexRight = right;
	mTexBottom = bottom;
	needupdate();
}

void ElScreenElement::setTexture(IDirect3DTexture9* texture)
{
	mTexture = texture;
}

void ElScreenElement::needupdate()
{
	mNeedUpdate = true;
}

unsigned int ElScreenElement::getWidth()
{
	const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();
	return static_cast<unsigned int>(mWidth * backBufferDesc.Width);
}

unsigned int ElScreenElement::getHeight()
{
	const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();
	return static_cast<unsigned int>(mHeight * backBufferDesc.Height);
}

void ElScreenElement::render()
{
	_update();

	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->SetTexture(0, mTexture);
	d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertices.base(), sizeof(VERTEX));
	d3dDevice->SetTexture(0, NULL);
}

void ElScreenElement::_update()
{
	if (mNeedUpdate)
	{
		const D3DSURFACE_DESC& backBufferDesc = ElDeviceManager::getSingleton().getBackBufferDesc();

		float xstart = (mLeft - 0.5f) * backBufferDesc.Width;
		float ystart = (0.5f - mTop) * backBufferDesc.Height;
		float xend = xstart + mWidth * backBufferDesc.Width;
		float yend = ystart - mHeight * backBufferDesc.Height;

		mVertices.assign(4);
		mVertices[0].pos = D3DXVECTOR3(xend, ystart, 0.0f);
		mVertices[0].color = mTopRightColor;
		mVertices[0].tex = D3DXVECTOR2(mTexRight, mTexTop);
		mVertices[1].pos = D3DXVECTOR3(xend, yend, 0.0f);
		mVertices[1].color = mBottomRightColor;
		mVertices[1].tex = D3DXVECTOR2(mTexRight, mTexBottom);
		mVertices[2].pos = D3DXVECTOR3(xstart, ystart, 0.0f);
		mVertices[2].color = mTopLeftColor;
		mVertices[2].tex = D3DXVECTOR2(mTexLeft, mTexTop);
		mVertices[3].pos = D3DXVECTOR3(xstart, yend, 0.0f);
		mVertices[3].color = mBottomLeftColor;
		mVertices[3].tex = D3DXVECTOR2(mTexLeft, mTexBottom);

		mNeedUpdate = false;
	}
}
