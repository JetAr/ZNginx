#ifndef __ElSkeletonSerializer_H__
#define __ElSkeletonSerializer_H__

#include "ElD3DHeaders.h"
#include "ElSkeletalAnimation.h"
#include "ElSkeleton.h"
#include "ElSingleton.h"
#include <fstream>
#include <string>

class ElSkeletonSerializer
{
public:
	ElSkeletonSerializer();
	virtual ~ElSkeletonSerializer();

	bool loadSkeleton(LPCSTR filename, ElSkeleton* skeleton);
	bool loadSkeletalAnimation(LPCSTR filename, ElSkeletalAnimation* skeletalAnim);

protected:
	void importGlobalInfo();
	void importBoneList(ElSkeleton* skeleton);
	void importSkeletalAnim(ElSkeletalAnimation* skeletalAnim);
	void importMatrix(D3DXMATRIX& m);

protected:
	std::ifstream infile;
	std::string token;
};

#endif //__ElSkeletonSerializer_H__