#include "ElApplicationPCH.h"
#include "ElSkeletonManager.h"

ElImplementSingleton(ElSkeletonManager);

ElSkeletonManager::ElSkeletonManager()
{

}

ElSkeletonManager::~ElSkeletonManager()
{
	SkeletonIterator i;
	for (i = mSkeletonList.begin(); i != mSkeletonList.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mSkeletonList.clear();
}

ElSkeleton* ElSkeletonManager::createSkeleton(LPCSTR filename)
{
	ElSkeleton* skeleton = getSkeleton(filename);
	if (skeleton == NULL)
	{
		skeleton = ElNew ElSkeleton;
		if (!skeleton->load(filename))
		{
			ElSafeDelete(skeleton);
		}
		else
			mSkeletonList.insert(SkeletonValue(skeleton->getName(), skeleton));
	}

	return skeleton;
}

ElSkeleton* ElSkeletonManager::getSkeleton(const std::string& name)
{
	SkeletonIterator i = mSkeletonList.find(name);
	if (i != mSkeletonList.end())
		return i->second;
	else
		return NULL;
}