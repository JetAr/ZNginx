#include "ElApplicationPCH.h"
#include "ElSkeletalAnimation.h"
#include "ElSkeletonSerializer.h"

ElSkeletalAnimation::ElSkeletalAnimation()
{

}

ElSkeletalAnimation::~ElSkeletalAnimation()
{
	int numFrames = mFrameList.size();
	for (int i = 0; i < numFrames; ++i)
	{
		ElSafeDelete(mFrameList[i]);
	}
	mFrameList.clear();
}

bool ElSkeletalAnimation::load(LPCTSTR filename)
{
	ElSkeletonSerializer serializer;
	return serializer.loadSkeletalAnimation(filename, this);
}

void ElSkeletalAnimation::addFrame(Frame* frame)
{
	mFrameList.push_back(frame);
}

ElSkeletalAnimation::Frame* ElSkeletalAnimation::getFrame(int idx)
{
	return mFrameList[idx];
}

int ElSkeletalAnimation::getNumFrames()
{
	return mFrameList.size();
}
