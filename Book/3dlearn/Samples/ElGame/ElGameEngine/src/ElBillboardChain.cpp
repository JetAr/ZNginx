#include "ElApplicationPCH.h"
#include "ElBillboardChain.h"
#include "ElDeviceManager.h"
#include "ElRenderQueue.h"
#include "ElSceneNode.h"

ElBillboardChain::Element::Element()
: position(0.0f, 0.0f, 0.0f)
, width(1.0f)
, texCoord(0.0f)
, colour(D3DCOLOR_ARGB(255, 255, 255, 255))
{

}

ElBillboardChain::Element::Element(D3DXVECTOR3 _position,
								   float _width,
								   float _texCoord,
								   ColorValue _colour)
: position(_position)
, width(_width)
, texCoord(_texCoord)
, colour(_colour)
{

}

ElBillboardChain::ElBillboardChain()
: mNeedUpdateBounding(true)
, mNeedUpdateVertices(true)
, mMaxElements(0)
, mTexture(NULL)
, mTexCoordMode(TextCoord_CustomX)
, mUseChainDirection(true)
{
	mRenderQueueID = RENDER_QUEUE_TRANSPARENT_1;
	mAABB.setNull();
}

ElBillboardChain::~ElBillboardChain()
{

}

void ElBillboardChain::setMaxChainElements(size_t maxElements)
{
	mMaxElements = maxElements;
	setupChainContainers();
	mNeedUpdateVertices = true;
}

void ElBillboardChain::addChainElement(const ElBillboardChain::Element& billboardChainElement)
{
	mChainElementList.push_back(billboardChainElement);
	needUpdate();
}

void ElBillboardChain::removeChainElement(size_t elementIndex)
{
	mChainElementList.erase(mChainElementList.begin() + elementIndex);
	needUpdate();
}

size_t ElBillboardChain::getNumChainElements() const
{
	return mChainElementList.size();
}

void ElBillboardChain::updateChainElement(size_t elementIndex, const ElBillboardChain::Element& billboardChainElement)
{
	mChainElementList[elementIndex] = billboardChainElement;
	needUpdate();
}

const ElBillboardChain::Element& ElBillboardChain::getChainElement(size_t elementIndex) const
{
	return mChainElementList[elementIndex];
}

void ElBillboardChain::clearChainElements()
{
	mChainElementList.clear();
	needUpdate();
}

void ElBillboardChain::setTexture(IDirect3DTexture9* texture)
{
	mTexture = texture;
}

void ElBillboardChain::setTextureCoordMode(unsigned char mode)
{
	assert(mTexCoordMode >= TextCoord_CustomX && mTexCoordMode <= TextCoord_CustomY);
	mTexCoordMode = mode;
	mNeedUpdateVertices = true;
}

void ElBillboardChain::setUseChainDirection(bool setting)
{
	mUseChainDirection = setting;
	needUpdate();
}

void ElBillboardChain::needUpdate()
{
	mNeedUpdateBounding = true;
	mNeedUpdateVertices = true;

	// tell parent node to update bounds
	if (mParentNode)
		mParentNode->needUpdate();
}

void ElBillboardChain::update()
{
	if (mNeedUpdateBounding)
	{
		_setupBoundingBox();
		mNeedUpdateBounding = false;
	}

	// provisionally, we assume the camera changes every frame, so we
	// need to update vertices every frame, actually we should only do
	// this after when really changed
	mNeedUpdateVertices = true;
}

void ElBillboardChain::render(ElCamera* camera, TimeValue t)
{
	if (mNeedUpdateVertices)
	{
		_setupVertices(camera);
		mNeedUpdateVertices = false;
	}

	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		D3DXMATRIX mat;
		getParentNodeTransform(mat);
		d3dDevice->SetTransform(D3DTS_WORLD, &mat);

		d3dDevice->SetFVF(VERTEX::FVF);
		d3dDevice->SetTexture(0, mTexture);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		int numTriangles = mChainElementList.size();
		numTriangles = max(0, (numTriangles - 1) * 2);
		d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, numTriangles, mVertices.base(), sizeof(VERTEX));

		d3dDevice->EndScene();
		d3dDevice->SetTexture(0, NULL);
		d3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
}

void ElBillboardChain::_updateRenderQueue(ElRenderQueue* queue, ElCamera* cam)
{
	// Check self visible
	if (!isVisible())
		return;

	// Add self to the queue
	queue->addRenderable(this, mRenderQueueID);
}

float ElBillboardChain::getSquaredViewDepth(ElCamera* cam) const
{
	// we only need this for transparent objects
	float dist = 0.0f;
	if (mParentNode)
		dist = D3DXVec3LengthSq(&(mParentNode->getDerivedPosition() - cam->getRealPosition()));
	return dist;
}

const ElAxisAlignedBox& ElBillboardChain::getBoundingBox()
{
	return mAABB;
}

void ElBillboardChain::setupChainContainers()
{
	mChainElementList.clear();
	mVertices.assign(mMaxElements * 2);
}

void ElBillboardChain::_setupVertices(ElCamera* cam)
{
	D3DXVECTOR3 chainDir(0.0f, 0.0f, 0.0f);
	if (!mChainElementList.empty())
		chainDir = mChainElementList.back().position - mChainElementList.begin()->position;
	int numElements = (int)mChainElementList.size();
	int lastElement = numElements - 1;
	for (int i = 0; i < numElements; ++i)
	{
		Element prevElement, nextElement;
		
		if (i != lastElement)
		{
			prevElement = mChainElementList[i];
			nextElement = mChainElementList[i + 1];
		}
		else if (i != 0)
		{
			prevElement = mChainElementList[i - 1];
			nextElement = mChainElementList[i];
		}
		else
		{
			prevElement = nextElement = mChainElementList[i];
		}

		D3DXVECTOR3 segmentDir;
		if (mUseChainDirection)
			segmentDir = chainDir;
		else
			segmentDir = nextElement.position - prevElement.position;

		D3DXVECTOR3 lookAt(0.0f, 0.0f, 1.0f);
		if (cam)
			lookAt = -cam->getDirection();

		D3DXVECTOR3 strechDir;
		D3DXVec3Cross(&strechDir, &lookAt, &segmentDir);
		D3DXVec3Normalize(&strechDir, &strechDir);

		// each element can be extended to two vertices, according to positions of
		// its neighbors and itself
		Element currentElement = mChainElementList[i];

		int vertexIdx = i * 2;
		mVertices[vertexIdx].pos = currentElement.position + (currentElement.width * strechDir);
		mVertices[vertexIdx].color = currentElement.colour;
		if (mTexCoordMode == TextCoord_CustomX)
			mVertices[vertexIdx].tex = D3DXVECTOR2(currentElement.texCoord, 1.0f);
		else if (mTexCoordMode == TextCoord_CustomY)
			mVertices[vertexIdx].tex = D3DXVECTOR2(1.0f, currentElement.texCoord);

		++vertexIdx;
		mVertices[vertexIdx].pos = currentElement.position - (currentElement.width * strechDir);
		mVertices[vertexIdx].color = currentElement.colour;
		if (mTexCoordMode == TextCoord_CustomX)
			mVertices[vertexIdx].tex = D3DXVECTOR2(currentElement.texCoord, 0.0f);
		else if (mTexCoordMode == TextCoord_CustomY)
			mVertices[vertexIdx].tex = D3DXVECTOR2(0.0f, currentElement.texCoord);
	}
}

void ElBillboardChain::_setupBoundingBox()
{
	mAABB.setNull();
	int numElements = mChainElementList.size();
	for (int i = 0; i < numElements; ++i)
	{
		mAABB.merge(mChainElementList[i].position);
	}
}


