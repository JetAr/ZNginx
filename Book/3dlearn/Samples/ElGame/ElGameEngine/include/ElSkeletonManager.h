#ifndef __ElSkeletonManager_H__
#define __ElSkeletonManager_H__

#include "ElSingleton.h"
#include "ElSkeleton.h"
#include <map>
#include <string>

class ElSkeletonManager
{
	ElDeclareSingleton(ElSkeletonManager);

public:
	typedef std::map<std::string, ElSkeleton*> SkeletonMap;
	typedef SkeletonMap::iterator SkeletonIterator;
	typedef SkeletonMap::const_iterator SkeletonConstIterator;
	typedef SkeletonMap::value_type SkeletonValue;

	ElSkeletonManager();
	virtual ~ElSkeletonManager();

	ElSkeleton*		createSkeleton(LPCSTR filename);
	ElSkeleton*		getSkeleton(const std::string& name);

protected:
	SkeletonMap		mSkeletonList;
};

#endif //__ElSkeletonManager_H__