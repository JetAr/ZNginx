#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElSceneNode.h"
#include "ElSceneManager.h"
#include "ElCamera.h"

ElSceneNode::ElSceneNode()
: mPosition(0.0f, 0.0f, 0.0f)
, mScale(1.0f, 1.0f, 1.0f)
, mDerivedPosition(0.0f, 0.0f, 0.0f)
, mDerivedScale(1.0f, 1.0f, 1.0f)
, mNeedUpdate(false)
, mInheritOrientation(true)
, mInheritScale(true)
, mShowBoundingBox(false)
, mBoundingBoxColor(D3DCOLOR_XRGB(255, 255, 255))
, mWireBoundingBox(NULL)
, mListener(NULL)
{
	D3DXQuaternionIdentity(&mOrientation);
	D3DXQuaternionIdentity(&mDerivedOrientation);

	needUpdate();
}

ElSceneNode::ElSceneNode(const std::string& name)
: mPosition(0.0f, 0.0f, 0.0f)
, mScale(1.0f, 1.0f, 1.0f)
, mDerivedPosition(0.0f, 0.0f, 0.0f)
, mDerivedScale(1.0f, 1.0f, 1.0f)
, mNeedUpdate(false)
, mInheritOrientation(true)
, mInheritScale(true)
, ElNode(name)
, mShowBoundingBox(false)
, mBoundingBoxColor(D3DCOLOR_XRGB(255, 255, 255))
, mWireBoundingBox(NULL)
, mListener(NULL)
{
	D3DXQuaternionIdentity(&mOrientation);
	D3DXQuaternionIdentity(&mDerivedOrientation);

	needUpdate();
}

ElSceneNode::~ElSceneNode()
{
	// Call listener (note, only called if there's something to do)
	if (mListener)
	{
		mListener->nodeDestroyed(this);
	}

	ObjectIterator i;
	for (i = mObjectsByName.begin(); i != mObjectsByName.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mObjectsByName.clear();

	ElSafeDelete(mWireBoundingBox);
}

void ElSceneNode::setParent(ElSceneNode* parent)
{
	bool different = (parent != mParent);

	ElNode::setParent(parent);
	needUpdate();

	// Call listener (note, only called if there's something to do)
	if (mListener && different)
	{
		if (mParent)
			mListener->nodeAttached(this);
		else
			mListener->nodeDetached(this);
	}
}

const D3DXQUATERNION& ElSceneNode::getOrientation()
{
	return mOrientation;
}

const D3DXVECTOR3& ElSceneNode::getPosition()
{
	return mPosition;
}

const D3DXVECTOR3& ElSceneNode::getScale()
{
	return mScale;
}

void ElSceneNode::setOrientation(const D3DXQUATERNION& q)
{
	mOrientation = q;
	needUpdate();
}

void ElSceneNode::setScale(const D3DXVECTOR3& scale)
{
	mScale = scale;
	needUpdate();
}

void ElSceneNode::setPosition(const D3DXVECTOR3& pos)
{
	mPosition = pos;
	needUpdate();
}

void ElSceneNode::scale(float x, float y, float z)
{
	mScale.x = x;
	mScale.y = y;
	mScale.z = z;
	needUpdate();
}

void ElSceneNode::pitch(float angle)
{
	rotate(D3DXVECTOR3(1.0f, 0.0f, 0.0f), angle);
}

void ElSceneNode::yaw(float angle)
{
	rotate(D3DXVECTOR3(0.0f, 1.0f, 0.0f), angle);
}

void ElSceneNode::roll(float angle)
{
	rotate(D3DXVECTOR3(0.0f, 0.0f, 1.0f), angle);
}

void ElSceneNode::rotate(const D3DXVECTOR3& axis, const float angle)
{
	D3DXQUATERNION q;
	D3DXQuaternionRotationAxis(&q, &axis, angle);
	D3DXQuaternionNormalize(&q, &q);
	mOrientation = mOrientation * q;
	needUpdate();
}

const D3DXQUATERNION& ElSceneNode::getDerivedOrientation() const
{
	return mDerivedOrientation;
}

const D3DXVECTOR3& ElSceneNode::getDerivedScale() const
{
	return mDerivedScale;
}

const D3DXVECTOR3& ElSceneNode::getDerivedPosition() const
{
	return mDerivedPosition;
}

const D3DXMATRIX& ElSceneNode::getCachedTransform() const
{
	return mCachedTransform;
}

bool ElSceneNode::getInheritOrientation() const
{
	return mInheritOrientation;
}

bool ElSceneNode::getInheritScale() const
{
	return mInheritScale;
}

void ElSceneNode::setInheritOrientation(bool inherit)
{
	mInheritOrientation = inherit;
	needUpdate();
}

void ElSceneNode::setInheritScale(bool inherit)
{
	mInheritScale = inherit;
	needUpdate();
}

void ElSceneNode::needUpdate()
{
	mNeedUpdate = true;

	ElSceneNode* child = 0;
	for (ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i)
		static_cast<ElSceneNode*>(i->second)->needUpdate();
}

void ElSceneNode::update()
{
	if (mNeedUpdate)
	{
		_updateFromParent();
		_makeCachedTransform();
		
		mNeedUpdate = false;
	}

	// update attached objects
	for (ObjectIterator i = mObjectsByName.begin(); i != mObjectsByName.end(); ++i)
		i->second->update();

	// update children
	for (ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i)
		static_cast<ElSceneNode*>(i->second)->update();

	// update bounding box
	_updateBounds();
}

void ElSceneNode::_findVisibleObjects(ElCamera* cam, ElRenderQueue* queue, bool includeChildren /* = true */)
{
	// Check self visible
	if (cam && !cam->isVisible(mWorldAABB))
		return;

	// Add all entities
	for (ObjectIterator i = mObjectsByName.begin(); i != mObjectsByName.end(); ++i)
	{
		ElMovableObject* mo = i->second;
		queue->processVisibleObject(mo, cam);
	}

	if (includeChildren)
	{
		for (ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i)
		{
			ElSceneNode* sceneChild = static_cast<ElSceneNode*>(i->second);
			static_cast<ElSceneNode*>(i->second)->_findVisibleObjects(cam, queue, includeChildren);
		}
	}

	// Check if the bounding box should be shown.
	if (mShowBoundingBox)
	{
		_addBoundingBoxToQueue(queue);
	}
}

void ElSceneNode::attachObject(ElMovableObject* obj)
{
	if (obj->isAttached())
		assert(0);

	obj->setAttached(this);

	// Also add to name index
	std::pair<ObjectMap::iterator, bool> insresult =
		mObjectsByName.insert(ObjectValue(obj->getName(), obj));
	assert(insresult.second && "Object was not attached because an object of the "
		"same name was already attached to this node.");

	needUpdate();
}

void ElSceneNode::dettachObject(ElMovableObject* obj)
{
	ObjectIterator i, iend;
	iend = mObjectsByName.end();
	for (i = mObjectsByName.begin(); i != iend; ++i)
	{
		if (i->second == obj)
		{
			mObjectsByName.erase(i);
			break;
		}
	}

	needUpdate();
}

ElSceneNode* ElSceneNode::createChildSceneNode(const std::string& name)
{
	if (getChild(name))
	{
		assert(0 && "a child of the same name was already attached to this node");
		return NULL;
	}

	ElSceneNode* newChild = ElNew ElSceneNode(name);
	this->addChild(newChild);
	return newChild;
}

const ElAxisAlignedBox& ElSceneNode::getWorldAABB() const
{
	return mWorldAABB;
}

void ElSceneNode::showBoundingBox(bool show)
{
	mShowBoundingBox = show;
}

void ElSceneNode::setBoundingBoxColor(ColorValue color)
{
	mBoundingBoxColor = color;
}

void ElSceneNode::setVisible(bool visible, bool cascade /* = true */)
{
	for (ObjectIterator i = mObjectsByName.begin(); i != mObjectsByName.end(); ++i)
		i->second->setVisible(visible);

	if (cascade)
	{
		for (ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i)
			static_cast<ElSceneNode*>(i->second)->setVisible(visible, cascade);
	}
}

void ElSceneNode::intersects(ElRaySceneQuery& raySceneQuery)
{
	// intersects self first
	std::pair<bool, float> result = raySceneQuery.getRay().intersects(mWorldAABB);
	if (!result.first)
		return;

	// intersects attached objects
	for (ObjectIterator i = mObjectsByName.begin(); i != mObjectsByName.end(); ++i)
		i->second->intersects(raySceneQuery);

	// intersects children
	for (ChildNodeIterator i = mChildren.begin(); i != mChildren.end(); ++i)
		static_cast<ElSceneNode*>(i->second)->intersects(raySceneQuery);
}

void ElSceneNode::setListener(ElSceneNode::Listener* listener)
{
	mListener = listener;
}

ElSceneNode::Listener* ElSceneNode::getListener() const
{
	return mListener;
}

void ElSceneNode::_updateFromParent()
{
	updateFromParentImpl();

	// Call listener (note, this method only called if there's something to do)
	if (mListener)
	{
		mListener->nodeUpdated(this);
	}
}

void ElSceneNode::updateFromParentImpl()
{
	if (mParent)
	{
		ElSceneNode* parent = static_cast<ElSceneNode*>(mParent);

		// update orientation
		const D3DXQUATERNION& parentOrientation = parent->getDerivedOrientation();
		if (mInheritOrientation)
		{
			// Combine orientation with that of parent
			mDerivedOrientation = parentOrientation * mOrientation;
		}
		else
		{
			// No inheritence
			mDerivedOrientation = mOrientation;
		}

		// update scale
		const D3DXVECTOR3& parentScale = parent->getDerivedScale();
		if (mInheritScale)
		{
			// Scale own position by parent scale, NB just combine
			// as equivalent axes, no shearing
			mDerivedScale.x = parentScale.x * mScale.x;
			mDerivedScale.y = parentScale.x * mScale.y;
			mDerivedScale.z = parentScale.x * mScale.z;
		}
		else
		{
			// No inheritence
			mDerivedScale = mScale;
		}

		// Change position vector based on parent's orientation & scale
		D3DXVECTOR3 pos;
		pos.x = parentScale.x * mPosition.x;
		pos.y = parentScale.y * mPosition.y;
		pos.z = parentScale.z * mPosition.z;
		D3DXMATRIX m;
		D3DXMatrixRotationQuaternion(&m, &parentOrientation);
		D3DXVec3TransformCoord(&mDerivedPosition, &pos, &m);

		// Add altered position vector to parents
		mDerivedPosition += parent->getDerivedPosition();
	}
	else
	{
		// Root node, no parent
		mDerivedOrientation = mOrientation;
		mDerivedPosition = mPosition;
		mDerivedScale = mScale;
	}
}

void ElSceneNode::_makeCachedTransform()
{
	D3DXMATRIX sm, rm, tm;
	D3DXMatrixScaling(&sm, mDerivedScale.x, mDerivedScale.y, mDerivedScale.z);
	D3DXMatrixRotationQuaternion(&rm, &mDerivedOrientation);
	D3DXMatrixTranslation(&tm, mDerivedPosition.x, mDerivedPosition.y, mDerivedPosition.z);
	mCachedTransform = sm * rm * tm;
}

void ElSceneNode::_addBoundingBoxToQueue(ElRenderQueue* queue)
{
	if (mWireBoundingBox == NULL)
		mWireBoundingBox = ElNew ElWireBoundingBox;
	mWireBoundingBox->setupBoundingBoxVertices(mWorldAABB);
	mWireBoundingBox->setBoundingBoxColor(mBoundingBoxColor);
	queue->addRenderable(mWireBoundingBox, RENDER_QUEUE_MAIN);
}

void ElSceneNode::_updateBounds()
{
	// Reset bounds first
	mWorldAABB.setNull();

	// Update bounds from own attached objects
	for (ObjectMap::iterator i = mObjectsByName.begin(); i != mObjectsByName.end(); ++i)
	{
		// Merge world bounds of each object
		mWorldAABB.merge(i->second->getWorldBoundingBox(true));
	}

	// Merge with children
	for (ChildNodeMap::iterator child = mChildren.begin(); child != mChildren.end(); ++child)
	{
		ElSceneNode* sceneChild = static_cast<ElSceneNode*>(child->second);
		mWorldAABB.merge(sceneChild->mWorldAABB);
	}
}
