#include "ElApplicationPCH.h"
#include "ElMovableObject.h"
#include "ElDefines.h"
#include "ElSceneNode.h"
#include "ElNameGenerator.h"
#include "ElCamera.h"
#include "ElRaySceneQuery.h"

ElMovableObject::ElMovableObject()
: mParentNode(NULL)
, mName("")
, mVisible(true)
, mRenderQueueID(RENDER_QUEUE_MAIN)
{
	mName = ElNameGenerator::getSingleton().generateMovableObjectName();
};

ElMovableObject::~ElMovableObject()
{

}

void ElMovableObject::update()
{

}

void ElMovableObject::render(ElCamera* camera, TimeValue t)
{

}

void ElMovableObject::setName(const std::string& name)
{
	mName = name;
}

const std::string& ElMovableObject::getName()
{
	return mName;
}

void ElMovableObject::setAttached(ElSceneNode* parent)
{
	mParentNode = parent;
}

bool ElMovableObject::isAttached()
{
	return mParentNode != NULL;
}

const ElAxisAlignedBox& ElMovableObject::getWorldBoundingBox(bool derive /* = false */)
{
	if (derive)
	{
		mWorldAABB = getBoundingBox();
		D3DXMATRIX mat;
		getParentNodeTransform(mat);
		mWorldAABB.transformAffine(mat);
	}

	return mWorldAABB;
}

const ElAxisAlignedBox& ElMovableObject::getBoundingBox()
{
	return mWorldAABB;
}

ElSceneNode* ElMovableObject::getParentNode()
{
	return mParentNode;
}

void ElMovableObject::getParentNodeTransform(D3DXMATRIX& mat)
{
	if (mParentNode)
		mat = mParentNode->getCachedTransform();
	else
		D3DXMatrixIdentity(&mat);
}

void ElMovableObject::setVisible(bool visible)
{
	mVisible = visible;
}

bool ElMovableObject::isVisible() const
{
	return mVisible;
}

void ElMovableObject::setRenderQueueGroup(unsigned int queueID)
{
	mRenderQueueID = queueID;
}

unsigned int ElMovableObject::getRenderQueueGroup() const
{
	return mRenderQueueID;
}

void ElMovableObject::intersects(ElRaySceneQuery& raySceneQuery)
{
	if (!isVisible())
		return;

	std::pair<bool, float> result = raySceneQuery.getRay().intersects(getWorldBoundingBox(true));
	if (result.first)
		raySceneQuery.queryResult(this, result.second);
}