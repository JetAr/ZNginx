#include "ElApplicationPCH.h"
#include "ElMesh.h"
#include "ElDefines.h"
#include "ElMeshSerializer.h"
#include "ElNameGenerator.h"

ElHelperObject::ElHelperObject()
: mName("")
, mParentName("")
, mClassName("")
, mScale(1.0f, 1.0f, 1.0f)
, mPosition(0.0f, 0.0f, 0.0f)
, mBoundingBoxMin(0.0f, 0.0f, 0.0f)
, mBoundingBoxMax(0.0f, 0.0f, 0.0f)
{
	D3DXQuaternionIdentity(&mOrientation);
}

ElHelperObject::~ElHelperObject()
{

}

ElMesh::ElMesh()
: mSkeleton(NULL)
, mName("")
{

}

ElMesh::~ElMesh()
{
	int numMesh = mSubMeshList.size();
	for (int i = 0; i < numMesh; ++i)
	{
		ElSafeDelete(mSubMeshList[i]);
	}
	mSubMeshList.clear();

	int numHelperObj = mHelperObjList.size();
	for (int i = 0; i < numHelperObj; ++i)
	{
		ElSafeDelete(mHelperObjList[i]);
	}
	mHelperObjList.clear();

	for (AnimationIterator i = mAnimationList.begin(); i != mAnimationList.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mAnimationList.clear();
}

bool ElMesh::load(LPCSTR filename)
{
	ElMeshSerializer serializer;
	if (!serializer.loadMesh(filename, this))
		return false;
	
	// for now, we just set the filename as the mesh name
	mName = filename;

	return true;
}

void ElMesh::setSubMeshList(ElSubMeshList& list)
{
	mSubMeshList.clear();
	mSubMeshList.insert(mSubMeshList.end(), list.begin(), list.end());

	// buildBounds according to subMeshes
	buildBounds();
}

void ElMesh::setHelperObjList(ElHelperObjectList& list)
{
	mHelperObjList.clear();
	mHelperObjList.insert(mHelperObjList.end(), list.begin(), list.end());
}

void ElMesh::attachSkeleton(ElSkeleton* skeleton)
{
	if (mSkeleton != NULL)
	{
		// This mesh has been attached to a skeleton, in this case, we will
		// simply do nothing but return, if you want to attach this mesh to
		// another skeleton, you should call dettachSkeleton() first
		return;
	}

	// set skeleton
	mSkeleton = skeleton;

	// update sub meshes
	int numMesh = mSubMeshList.size();
	for (int i = 0; i < numMesh; ++i)
		mSubMeshList[i]->attachSkeleton(skeleton);
}

void ElMesh::dettachSkeleton()
{
	if (mSkeleton)
	{
		mSkeleton = NULL;
		int numMesh = mSubMeshList.size();
		for (int i = 0; i < numMesh; ++i)
			mSubMeshList[i]->dettachSkeleton();
	}
}

const std::string& ElMesh::getName()
{
	return mName;
}

int ElMesh::getNumSubMeshes()
{
	return mSubMeshList.size();
}

ElSubMesh* ElMesh::getSubMesh(int idx)
{
	return mSubMeshList[idx];
}

ElSubMesh* ElMesh::getSubMesh(const std::string name)
{
	int numMesh = mSubMeshList.size();
	for (int i = 0; i < numMesh; ++i)
	{
		if (mSubMeshList[i]->getName().compare(name) == 0)
			return mSubMeshList[i];
	}

	return NULL;
}

int ElMesh::getNumHelperObjects()
{
	return mHelperObjList.size();
}

ElHelperObject* ElMesh::getHelperObject(int idx)
{
	return mHelperObjList[idx];
}

bool ElMesh::hasMeshAnimation()
{
	return !mAnimationList.empty();
}

bool ElMesh::createAnimation(LPCSTR filename, const std::string& name)
{
	// see if a animation with the same name already exists
	if (getAnimation(name))
		return true;

	ElMeshAnimation* meshAnim = ElNew ElMeshAnimation;
	if (!meshAnim->load(filename))
	{
		ElSafeDelete(meshAnim);
		return false;
	}

	mAnimationList.insert(AnimationValue(name, meshAnim));
	return true;
}

ElMeshAnimation* ElMesh::getAnimation(const std::string& name)
{
	AnimationIterator i = mAnimationList.find(name);
	if (i != mAnimationList.end())
		return i->second;
	else
		return NULL;
}

bool ElMesh::hasSkeleton()
{
	int numMesh = mSubMeshList.size();
	for (int i = 0; i < numMesh; ++i)
	{
		if (mSubMeshList[i]->hasSkeleton())
			return true;
	}

	return false;
}

ElSkeleton* ElMesh::getSkeleton()
{
	return mSkeleton;
}

void ElMesh::buildBounds()
{
	int numMesh = mSubMeshList.size();
	for (int i = 0; i < numMesh; ++i)
		mAABB.merge(mSubMeshList[i]->getBounds());
}

const ElAxisAlignedBox& ElMesh::getBounds() const
{
	return mAABB;
}

