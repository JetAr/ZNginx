#include "ElApplicationPCH.h"
#include "ElRibbonTrail.h"

ElRibbonTrail::ElRibbonTrail()
: mTrailWidth(1.0f)
, mTrailLength(0.0f)
, mElemLength(0.0f)
, mSquaredElemLength(0.0f)
, mTraceDirty(true)
{

}

ElRibbonTrail::~ElRibbonTrail()
{

}

void ElRibbonTrail::setTrailLength(float len)
{
	mTrailLength = len;
	mElemLength = mTrailLength / mMaxElements;
	mSquaredElemLength = mElemLength * mElemLength;
}

void ElRibbonTrail::setTrailWidth(float width)
{
	clearChainElements();
	mTrailWidth = width;
	mTraceDirty = true;
}

void ElRibbonTrail::setMaxChainElements(size_t maxElements)
{
	ElBillboardChain::setMaxChainElements(maxElements);
	mElemLength = mTrailLength / mMaxElements;
	mSquaredElemLength = mElemLength * mElemLength;

	resetTrail();
}

void ElRibbonTrail::updateTrail(const D3DXVECTOR3& position)
{
	bool update = false;
	if (mTraceList.empty())
		update = true;
	else
	{
		D3DXVECTOR3 diff = mTraceList[0] - position;
		if (D3DXVec3LengthSq(&diff) >= mSquaredElemLength)
			update = true;
		else
			update = false;
	}

	if (update)
	{
		while (mTraceList.size() >= mMaxElements)
			mTraceList.erase(mTraceList.begin());
		mTraceList.push_back(position);
		mTraceDirty = true;
	}
}

void ElRibbonTrail::resetTrail()
{
	mTraceList.clear();
	mTraceDirty = true;
}

void ElRibbonTrail::update()
{
	_updateChainElements();
	ElBillboardChain::update();
}

float ElRibbonTrail::getSquaredViewDepth(ElCamera* cam) const
{
	// we only need this for transparent objects
	float dist = 0.0f;
	if (!mTraceList.empty())
		dist = D3DXVec3LengthSq(&(mTraceList[0] - cam->getRealPosition()));
	return dist;
}

void ElRibbonTrail::attachNode(ElSceneNode* node)
{
	if (node->getListener())
	{
		assert(0 && std::string(mName + " cannot monitor node " + node->getName() + " since it already has a listener. RibbonTrail::addNode().").c_str());
	}

	// initialise the chain
	resetTrail();

	node->setListener(this);
}

void ElRibbonTrail::nodeUpdated(const ElSceneNode* node)
{
	updateTrail(node->getDerivedPosition());
}

void ElRibbonTrail::nodeDestroyed(const ElSceneNode* node)
{
	const_cast<ElSceneNode*>(node)->setListener(NULL);
}

void ElRibbonTrail::_createChainElements()
{
	clearChainElements();

	float texCoordScale = 1.0f / (mMaxElements - 1);
	for (size_t i = 0; i < mMaxElements; ++i)
	{
		Element e;
		e.width = mTrailWidth;
		e.texCoord = 1.0f - texCoordScale * (float)i;
		addChainElement(e);
	}
}

void ElRibbonTrail::_updateChainElements()
{
	if (mTraceDirty)
	{
		if (!mTraceList.empty())
		{
			if (mChainElementList.size() != mMaxElements)
				_createChainElements();

			int numTraces = (int)mTraceList.size() - 1;
			for (int i = 0; i < mMaxElements; ++i)
			{
				int idx = max(0, numTraces - i);
				mChainElementList[i].position = mTraceList[idx];
			}

			needUpdate();
		}

		mTraceDirty = false;
	}
}