#ifndef __ElBone_H__
#define __ElBone_H__

#include "ElD3DHeaders.h"
#include "ElSkeletalAnimation.h"
#include "ElNode.h"
#include <vector>

class ElBone : public ElNode
{
public:
	ElBone();
	virtual ~ElBone();

	virtual void	update();

public:
	std::string		mParentName;
	D3DXMATRIX		mInitTM;
	D3DXMATRIX		mInverseInitTM;

	// assigned after optimize
	D3DXMATRIX		mTransform;
};

typedef std::vector<ElBone*> ElBoneList;

#endif //__ElBone_H__