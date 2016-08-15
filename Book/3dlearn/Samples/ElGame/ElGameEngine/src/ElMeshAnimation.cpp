#include "ElApplicationPCH.h"
#include "ElMeshAnimation.h"
#include "ElMeshSerializer.h"

ElSubMeshAnimation::ElSubMeshAnimation()
{

}

ElSubMeshAnimation::~ElSubMeshAnimation()
{
	int numFrames = mFrameList.size();
	for (int i = 0; i < numFrames; ++i)
	{
		ElSafeDelete(mFrameList[i]);
	}
	mFrameList.clear();
}

void ElSubMeshAnimation::addFrame(Frame* frame)
{
	mFrameList.push_back(frame);
}

ElSubMeshAnimation::Frame* ElSubMeshAnimation::getFrame(int idx)
{
	return mFrameList[idx];
}

int ElSubMeshAnimation::getNumFrames()
{
	return mFrameList.size();
}

ElMeshAnimation::ElMeshAnimation()
{

}

ElMeshAnimation::~ElMeshAnimation()
{
	for (SubMeshAnimIterator i = mAnimationSet.begin(); i != mAnimationSet.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mAnimationSet.clear();
}

bool  ElMeshAnimation::load(LPCTSTR filename)
{
	ElMeshSerializer serializer;
	return serializer.loadMeshAnimation(filename, this);
}

bool ElMeshAnimation::addAnimation(const std::string& subMeshName, ElSubMeshAnimation* animation)
{
	if (mAnimationSet.find(subMeshName) != mAnimationSet.end())
	{
		assert(0 && "animation already exists with the same subMeshName");
		return false;
	}

	mAnimationSet.insert(SubMeshAnimValue(subMeshName, animation));
	return true;
}

ElSubMeshAnimation* ElMeshAnimation::getAnimation(const std::string& subMeshName)
{
	SubMeshAnimIterator i = mAnimationSet.find(subMeshName);
	if (i != mAnimationSet.end())
		return i->second;
	else
		return NULL;
}

ElSubMeshAnimation* ElMeshAnimation::getLongestAnimation()
{
	ElSubMeshAnimation* animation = 0;
	int len = 0;

	for (SubMeshAnimIterator i = mAnimationSet.begin(); i != mAnimationSet.end(); ++i)
	{
		int l = i->second->getNumFrames();
		if (l > len)
		{
			len = l;
			animation = i->second;
		}
	}
	
	return animation;
}
