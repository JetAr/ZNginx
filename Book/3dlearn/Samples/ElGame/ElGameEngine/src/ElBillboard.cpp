#include "ElApplicationPCH.h"
#include "ElBillboard.h"
#include "ElDeviceManager.h"
#include "ElCamera.h"
#include "ElSceneNode.h"

ElBillboard::ElBillboard()
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
, mTexRight(1.0f)
, mTexBottom(1.0f)
, mYawFixed(false)
{
	mRenderQueueID = RENDER_QUEUE_TRANSPARENT_1;

	mVertices.assign(4);
	needupdate();
}

ElBillboard::~ElBillboard()
{

}

void ElBillboard::setRectangle(float left, float top, float width, float height)
{
	mLeft = left;
	mTop = top;
	mWidth = width;
	mHeight = height;
	needupdate();
}

void ElBillboard::setColor(const D3DXCOLOR& top_left, const D3DXCOLOR& top_right, const D3DXCOLOR& bottom_left, const D3DXCOLOR& bottom_right)
{
	mTopLeftColor = top_left;
	mTopRightColor = top_right;
	mBottomLeftColor = bottom_left;
	mBottomRightColor = bottom_right;
	needupdate();
}

void ElBillboard::setTextures(float left, float top, float right, float bottom)
{
	mTexLeft = left;
	mTexTop = top;
	mTexRight = right;
	mTexBottom = bottom;
	needupdate();
}

void ElBillboard::setTexture(IDirect3DTexture9* texture)
{
	mTexture = texture;
}

void ElBillboard::setAlpha(float alpha)
{
	D3DXCOLOR color = mTopLeftColor;
	color.a = alpha;
	mTopLeftColor = color;

	color = mTopRightColor;
	color.a = alpha;
	mTopRightColor = color;

	color = mBottomLeftColor;
	color.a = alpha;
	mBottomLeftColor = color;

	color = mBottomRightColor;
	color.a = alpha;
	mBottomRightColor = color;

	needupdate();
}

void ElBillboard::setFixedYawAxis(bool fixed)
{
	mYawFixed = fixed;
}

void ElBillboard::needupdate()
{
	mNeedUpdate = true;
}

void ElBillboard::update()
{
	if (mNeedUpdate)
	{
		/*
                         y
                         |
          (left, top) ___|
                     |\  |
                     | \ |
                x____|__\|
		*/

		float xstart = mLeft;
		float ystart = mTop;
		float xend = xstart - mWidth;
		float yend = ystart - mHeight;

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

		mAABB.merge(mVertices[0].pos);
		mAABB.merge(mVertices[1].pos);
		mAABB.merge(mVertices[2].pos);
		mAABB.merge(mVertices[3].pos);

		mNeedUpdate = false;
	}
}

void ElBillboard::render(ElCamera* camera, TimeValue t)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		_setTransform(camera);
		
		d3dDevice->SetFVF(VERTEX::FVF);
		d3dDevice->SetTexture(0, mTexture);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, mVertices.base(), sizeof(VERTEX));

		d3dDevice->EndScene();
		d3dDevice->SetTexture(0, NULL);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
}

void ElBillboard::_updateRenderQueue(ElRenderQueue* queue, ElCamera* cam)
{
	// Check self visible
	if (!isVisible())
		return;

	// Add self to the queue
	queue->addRenderable(this, mRenderQueueID);
}

float ElBillboard::getSquaredViewDepth(ElCamera* cam) const
{
	// we only need this for transparent objects
	float dist = 0.0f;
	if (mParentNode)
		dist = D3DXVec3LengthSq(&(mParentNode->getDerivedPosition() - cam->getRealPosition()));
	return dist;
};

const ElAxisAlignedBox& ElBillboard::getBoundingBox()
{
	return mAABB;
}

void ElBillboard::_setTransform(ElCamera* camera)
{
	// make sure the billboard is always faced at the camera
	float dot = 0.0f;
	
	D3DXVECTOR3 lookAt(0.0f, 0.0f, 1.0f);
	if (camera)
		lookAt = -camera->getDirection();
	D3DXVECTOR3 adjustLookAt = lookAt;
	adjustLookAt.y = 0.0f;

	D3DXVec3Normalize(&lookAt, &lookAt);
	D3DXVec3Normalize(&adjustLookAt, &adjustLookAt);

	D3DXMATRIX rm_x;
	if (mYawFixed)
	{
		D3DXMatrixIdentity(&rm_x);
	}
	else
	{
		D3DXMATRIX rm_z;
		D3DXMatrixIdentity(&rm_z);
		if (camera && camera->getUp().y < 0.0f)
			D3DXMatrixRotationAxis(&rm_z, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DX_PI);

		dot = D3DXVec3Dot(&adjustLookAt, &lookAt);
		if (lookAt.y <= 0)
			D3DXMatrixRotationAxis(&rm_x, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), acos(dot));
		else
			D3DXMatrixRotationAxis(&rm_x, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f), acos(dot));

		rm_x = rm_z * rm_x;
	}

	D3DXMATRIX rm_y;
	dot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &adjustLookAt);
	if (adjustLookAt.x <= 0)
		D3DXMatrixRotationAxis(&rm_y, &D3DXVECTOR3(0.0f, -1.0f, 0.0f), acos(dot));
	else
		D3DXMatrixRotationAxis(&rm_y, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), acos(dot));
	
	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	if (mParentNode)
		pos = mParentNode->getDerivedPosition();
	D3DXMATRIX tm;
	D3DXMatrixTranslation(&tm, pos.x, pos.y, pos.z);

	D3DXMATRIX matrix = rm_x * rm_y * tm;

	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	d3dDevice->SetTransform(D3DTS_WORLD, &matrix);
}
