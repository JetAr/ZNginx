#include "ElApplicationPCH.h"
#include "ElSkeleton.h"
#include "ElSkeletonSerializer.h"

ElSkeleton::ElSkeleton()
: mName("")
, mBoneTree(NULL)
{

}

ElSkeleton::~ElSkeleton()
{
	ElSafeDelete(mBoneTree);

	int numBones = mBoneList.size();
	for (int b = 0; b < numBones; ++b)
	{
		ElSafeDelete(mBoneList[b]);
	}

	AnimationIterator i;
	for (i = mAnimationList.begin(); i != mAnimationList.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mAnimationList.clear();
}

void ElSkeleton::addBone(ElBone* bone)
{
	mBoneList.push_back(bone);
}

int ElSkeleton::getBoneID(const std::string& name)
{
	int numBones = mBoneList.size();
	for (int b = 0; b < numBones; ++b)
	{
		if (mBoneList[b]->getName() == name)
			return b;
	}

	return -1;
}

ElBone* ElSkeleton::getBone(int id)
{
	return mBoneList[id];
}

ElBone* ElSkeleton::getBone(const std::string& name)
{
	int id = getBoneID(name);
	if (id != -1)
		return getBone(id);
	else
		return NULL;
}

int ElSkeleton::getBoneCount()
{
	return mBoneList.size();
}

const std::string& ElSkeleton::getName()
{
	return mName;
}

bool ElSkeleton::load(LPCTSTR filename)
{
	ElSkeletonSerializer serializer;
	if (!serializer.loadSkeleton(filename, this))
		return false;

	// for now, we just set the filename as the skeleton name
	mName = filename;

	return true;
}

void ElSkeleton::optimize()
{
	// build a tree of bones
	mBoneTree = ElNew ElBone;

	int numBones = mBoneList.size();
	for (int b = 0; b < numBones; ++b)
	{
		ElBone* bone = mBoneList[b];
		if (bone->mParentName.empty())
			mBoneTree->addChild(bone);
		else
		{
			ElBone* parent = getBone(bone->mParentName);
			if (parent)
				parent->addChild(bone);
		}
	}
}

void ElSkeleton::update(ElSkeletalAnimation* sa, const ElAnimationState& as)
{
	if (sa)
	{
		ElSkeletalAnimation::Frame* startFrame = sa->getFrame(as.startIdx);
		ElSkeletalAnimation::Frame* endFrame = sa->getFrame(as.endIdx);

		// update all the bone tm first, according to the skinFrame lerping value
		int numBones = mBoneList.size();
		for (int b = 0; b < numBones; ++b)
		{
			// slerp by quaternion, for now, haven't seen any benefits
			ElDecomposedTM dm;
			D3DXVec3Lerp(&dm.scale, &startFrame->dm[b].scale, &endFrame->dm[b].scale, as.scale);
			D3DXQuaternionSlerp(&dm.rotation, &startFrame->dm[b].rotation, &endFrame->dm[b].rotation, as.scale);
			D3DXVec3Lerp(&dm.translation, &startFrame->dm[b].translation, &endFrame->dm[b].translation, as.scale);
			D3DXMATRIX sm, rm, tm;
			D3DXMatrixScaling(&sm, dm.scale.x, dm.scale.y, dm.scale.z);
			D3DXMatrixRotationQuaternion(&rm, &dm.rotation);
			D3DXMatrixTranslation(&tm, dm.translation.x, dm.translation.y, dm.translation.z);
			mBoneList[b]->mTransform = sm * rm * tm;

			//// lerp by matrix
			//mBoneList[b]->mTransform = startFrame->tm[b] + (endFrame->tm[b] - startFrame->tm[b]) * as.scale;
		}

		// update the tree
		mBoneTree->update();
	}
	else
	{
		// if we don't have a animation state, just set the bone's initTM as its transTM,
		// that will set the mesh to init state(first frame, somehow)
		D3DXMATRIX identityMatrix;
		D3DXMatrixIdentity(&identityMatrix);
		int numBones = mBoneList.size();
		for (int b = 0; b < numBones; ++b)
			mBoneList[b]->mTransform = identityMatrix;
	}
}

bool ElSkeleton::createAnimation(LPCSTR filename, const std::string& name)
{
	// see if a animation with the same name already exists
	if (getAnimation(name))
		return true;

	ElSkeletalAnimation* skeletalAnim = ElNew ElSkeletalAnimation;
	if (!skeletalAnim->load(filename))
	{
		ElSafeDelete(skeletalAnim);
		return false;
	}

	mAnimationList.insert(AnimationValue(name, skeletalAnim));
	return true;
}

ElSkeletalAnimation* ElSkeleton::getAnimation(const std::string& name)
{
	AnimationIterator i = mAnimationList.find(name);
	if (i != mAnimationList.end())
		return i->second;
	else
		return NULL;
}


