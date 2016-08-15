#ifndef __ElSkeleton_H__
#define __ElSkeleton_H__

#include "ElDefines.h"
#include "ElSkeletalAnimation.h"
#include "ElAnimation.h"
#include "ElBone.h"
#include <string>
#include <map>

class ElSkeleton
{
public:
	typedef std::map<std::string, ElSkeletalAnimation*> AnimationMap;
	typedef AnimationMap::iterator AnimationIterator;
	typedef AnimationMap::const_iterator AnimationConstIterator;
	typedef AnimationMap::value_type AnimationValue;

public:
	ElSkeleton();
	virtual ~ElSkeleton();

	void	addBone(ElBone* bone);
	int		getBoneID(const std::string& name);
	ElBone*	getBone(int id);
	ElBone*	getBone(const std::string& name);
	int		getBoneCount();

	const std::string& getName();
	
	bool	load(LPCTSTR filename);
	void	optimize();
	void	update(ElSkeletalAnimation* sa, const ElAnimationState& as);

	// skeleton animation set
	bool	createAnimation(LPCSTR filename, const std::string& name);
	ElSkeletalAnimation* getAnimation(const std::string& name);

protected:
	std::string				mName;

	ElBoneList				mBoneList;
	ElBone*					mBoneTree;

	AnimationMap			mAnimationList;
};

#endif //__ElSkeleton_H__