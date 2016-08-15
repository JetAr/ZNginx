#include "ElApplicationPCH.h"
#include "ElEntity.h"
#include "ElMeshManager.h"
#include "ElDeviceManager.h"
#include "ElSceneNode.h"
#include "ElCamera.h"

ElEntity::ElEntity(ElMesh* mesh)
: mMesh(mesh)
, mAnimation(NULL)
, mAlpha(1.0f)
, mUseSoftwareVP(false)
{

}

ElEntity::~ElEntity()
{
	for (ElSubEntityIterator i = mSubEntityList.begin(); i != mSubEntityList.end(); ++i)
	{
		ElSafeDelete(*i);
	}
	mSubEntityList.clear();

	ElSafeDelete(mAnimation);

	for (ChildObjectIterator i = mChildObjectList.begin(); i != mChildObjectList.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mChildObjectList.clear();
}

ElEntity* ElEntity::createEntity(LPCSTR filename)
{
	// Load mesh, if the mesh with the same filename already exists, the mesh manager will
	// do nothing, just return it
	ElMesh* mesh = ElMeshManager::getSingleton().createMesh(filename);
	if (mesh == NULL)
		return NULL;

	// Create entity, and link the mesh ref
	ElEntity* entity = ElNew ElEntity(mesh);

	// Build main subentity list
	entity->buildSubEntityList();
	
	// Build dummy object list
	entity->buildChildObjects();

	// Update of bounds of the parent SceneNode
	if (entity->mParentNode)
		entity->mParentNode->needUpdate();

	return entity;
}

void ElEntity::setAttached(ElSceneNode* parent)
{
	ElMovableObject::setAttached(parent);
	_notifyAttached(parent);
}

void ElEntity::attachSkeleton(ElSkeleton* skeleton)
{
	if (mMesh && mMesh->hasSkeleton())
	{
		mMesh->attachSkeleton(skeleton);
		_notifySkeletonAttached(skeleton);
	}
}

bool ElEntity::hasAnimation()
{
	return mAnimation != NULL;
}

float ElEntity::getAlpha() const
{
	return mAlpha;
}

void ElEntity::setAlpha(float alpha)
{
	mAlpha = alpha;
	if (float_equal(mAlpha, 1.0f))
		mRenderQueueID = RENDER_QUEUE_MAIN;
	else
		mRenderQueueID = RENDER_QUEUE_TRANSPARENT_1;
}

void ElEntity::render(ElCamera* camera, TimeValue t)
{
	// Since we know we're going to be rendered, take this opportunity to
	// update the animation
	updateAnimation(t);

	// set render state
	_setRenderStates();

	// render each sub entity in the list
	for (ElSubEntityIterator i = mSubEntityList.begin(); i != mSubEntityList.end(); ++i)
		(*i)->render(mAlpha);
	
	// reset render state
	_resetRenderStates();
}

void ElEntity::setAnimation(const std::string& name)
{
	// reset the animation set, if null, create one
	if (mAnimation)
		mAnimation->animation->reset();
	else
		mAnimation = ElNew AnimationSet;

	// parse animation from mesh frames if has any
	if (mMesh->hasMeshAnimation())
	{
		ElMeshAnimation* ma = mMesh->getAnimation(name);
		if (ma)
		{
			mAnimation->frames.meshFrames = ma;

			ElSubMeshAnimation* sma = ma->getLongestAnimation();
			if (sma)
			{
				int numFrames = sma->getNumFrames();
				for (int i = 0; i < numFrames; ++i)
					mAnimation->animation->addKeyFrameTime(sma->getFrame(i)->t);
			}

			for (ElSubEntityIterator i = mSubEntityList.begin(); i != mSubEntityList.end(); ++i)
			{
				sma = ma->getAnimation((*i)->getSubMesh()->getName());
				(*i)->setAnimation(sma);
			}
		}
	}
	// parse animation from skeletal frames if has any
	else if (mMesh->hasSkeleton() && mMesh->getSkeleton())
	{
		ElSkeletalAnimation* sa = mMesh->getSkeleton()->getAnimation(name);
		if (sa)
		{
			mAnimation->frames.skeletonFrames = sa;

			int numFrames = sa->getNumFrames();
			for (int i = 0; i < numFrames; ++i)
				mAnimation->animation->addKeyFrameTime(sa->getFrame(i)->t);
		}
	}
}

void ElEntity::playAnimation(TimeValue t, bool loop /* = true */)
{
	if (hasAnimation())
		mAnimation->animation->play(t, loop);
}

void ElEntity::setAnimationSpeed(float speed)
{
	if (hasAnimation())
		mAnimation->animation->setSpeed(speed);
}

void ElEntity::stopAnimation()
{
	if (hasAnimation())
		mAnimation->animation->stop();
}

const ElAxisAlignedBox& ElEntity::getWorldBoundingBox(bool derive /* = false */)
{
	if (derive)
	{
		mWorldAABB = getBoundingBox();
		D3DXMATRIX mat;
		getParentNodeTransform(mat);
		mWorldAABB.transformAffine(mat);

		mWorldAABB.merge(getChildWorldBoundingBox());
	}

	return mWorldAABB;
}

const ElAxisAlignedBox& ElEntity::getBoundingBox()
{
	if (mMesh)
	{
		mFullBoundingBox = mMesh->getBounds();
	}
	else
		mFullBoundingBox.setNull();

	return mFullBoundingBox;
}

ElSceneNode* ElEntity::getChildObject(const std::string& name)
{
	ChildObjectIterator i = mChildObjectList.find(name);
	if (i != mChildObjectList.end())
		return static_cast<ElSceneNode*>(i->second);
	else
		return NULL;
}

ElMesh* ElEntity::getMesh()
{
	return mMesh;
}

void ElEntity::_updateRenderQueue(ElRenderQueue* queue, ElCamera* cam)
{
	// Check self visible
	if (!isVisible())
		return;

	// Add self to the queue
	queue->addRenderable(this, mRenderQueueID);

	// queue each dummy object in the list
	for (ChildObjectIterator i = mChildObjectList.begin(); i != mChildObjectList.end(); ++i)
		i->second->_findVisibleObjects(cam, queue);
}

float ElEntity::getSquaredViewDepth(ElCamera* cam) const
{
	// we only need this for transparent objects
	float dist = 0.0f;
	if (mParentNode)
		dist = D3DXVec3LengthSq(&(mParentNode->getDerivedPosition() - cam->getRealPosition()));
	return dist;
};

void ElEntity::buildSubEntityList()
{
	ElSubEntity* subEntity;
	int numSubMeshes = mMesh->getNumSubMeshes();
	for (int i = 0; i < numSubMeshes; ++i)
	{
		subEntity = ElNew ElSubEntity(mMesh->getSubMesh(i));
		subEntity->initialise();
		mSubEntityList.push_back(subEntity);
	}
}

void ElEntity::buildChildObjects()
{
	ElHelperObject* helperObj;
	ElDummyObject* dummyObj;
	int numHelpers = mMesh->getNumHelperObjects();
	for (int i = 0; i < numHelpers; ++i)
	{
		helperObj = mMesh->getHelperObject(i);
		// for now, just handle the dummy helper objects
		if (helperObj->mClassName.compare("Dummy") == 0)
		{
			dummyObj = ElNew ElDummyObject(helperObj->mName);
			dummyObj->setParentName(helperObj->mParentName);
			dummyObj->setInitScale(helperObj->mScale);
			dummyObj->setInitQrientation(helperObj->mOrientation);
			dummyObj->setInitPosition(helperObj->mPosition);
			mChildObjectList.insert(ChildObjectValue(helperObj->mName, dummyObj));
		}
	}
}

void ElEntity::updateAnimation(TimeValue t)
{
	ElAnimationState as;
	if (hasAnimation())
	{
		mAnimation->animation->update(t);
		mAnimation->animation->getCurrentState(as);

		// mark the dummy objects dirty
		for (ChildObjectIterator i = mChildObjectList.begin(); i != mChildObjectList.end(); ++i)
			i->second->needUpdate();
	}

	// update the skeleton, if there is any
	if (mMesh->hasSkeleton() && mMesh->getSkeleton())
		mMesh->getSkeleton()->update(hasAnimation() ? mAnimation->frames.skeletonFrames : NULL, as);

	// update each sub entity in the list
	for (ElSubEntityIterator i = mSubEntityList.begin(); i != mSubEntityList.end(); ++i)
		(*i)->update(as);

	// update each dummy object in the list
	for (ChildObjectIterator i = mChildObjectList.begin(); i != mChildObjectList.end(); ++i)
		i->second->update();
}

void ElEntity::_setRenderStates()
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// set world matrix
	D3DXMATRIX mat;
	getParentNodeTransform(mat);
	d3dDevice->SetTransform(D3DTS_WORLD, &mat);

	// set vertex blend method and blend matrix, according to
	// attached skeleton if any
	if (mMesh->hasSkeleton() && mMesh->getSkeleton())
	{
		ElSkeleton* skeleton = mMesh->getSkeleton();
		int numBones = skeleton->getBoneCount();
		mUseSoftwareVP = ElDeviceManager::getSingleton().getDeviceCaps().MaxVertexBlendMatrixIndex + 1 < numBones;
		if (mUseSoftwareVP)
			d3dDevice->SetSoftwareVertexProcessing(TRUE);
		d3dDevice->SetRenderState(D3DRS_VERTEXBLEND, ElMaxVertexWeight - 1);
		d3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE);
		for (int b = 0; b < numBones; ++b)
		{
			ElBone* bone = skeleton->getBone(b);
			if (bone)
				d3dDevice->SetTransform(D3DTS_WORLDMATRIX(b), &(bone->mTransform * mat));
		}
	}
}

void ElEntity::_resetRenderStates()
{
	if (mMesh->hasSkeleton() && mMesh->getSkeleton())
	{
		IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
		d3dDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		d3dDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		if (mUseSoftwareVP)
			d3dDevice->SetSoftwareVertexProcessing(FALSE);
	}
}

void ElEntity::_notifyAttached(ElSceneNode* parent)
{
	for (ChildObjectIterator i = mChildObjectList.begin(); i != mChildObjectList.end(); ++i)
		i->second->setParent(parent);
}

void ElEntity::_notifySkeletonAttached(ElSkeleton* skeleton)
{
	// attach skeletal dummy objects to relative bones
	for (ChildObjectIterator i = mChildObjectList.begin(); i != mChildObjectList.end(); ++i)
	{
		ElBone* bone = skeleton->getBone(i->second->getParentName());
		if (bone)
			i->second->setAttachedBone(bone);
	}

	// notify skeleton attached to each subEntity in the list
	for (ElSubEntityIterator i = mSubEntityList.begin(); i != mSubEntityList.end(); ++i)
		(*i)->_notifySkeletonAttached(skeleton);
}

ElAxisAlignedBox ElEntity::getChildWorldBoundingBox()
{
	ElAxisAlignedBox aabb;
	aabb.setNull();

	ConstChildObjectIterator child_itr = mChildObjectList.begin();
	ConstChildObjectIterator child_itr_end = mChildObjectList.end();
	for(; child_itr != child_itr_end; ++child_itr)
	{
		aabb.merge(child_itr->second->getWorldAABB());
	}

	return aabb;
}
