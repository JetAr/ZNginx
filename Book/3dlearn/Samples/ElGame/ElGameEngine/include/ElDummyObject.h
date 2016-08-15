#ifndef __ElDummyObject_H__
#define __ElDummyObject_H__

#include "ElD3DHeaders.h"
#include "ElSceneNode.h"
#include "ElBone.h"
#include <string>
#include <map>

class ElDummyObject : public ElSceneNode
{
public:
	ElDummyObject(const std::string& name);
	virtual ~ElDummyObject();

	void setParentName(const std::string& name);
	const std::string& getParentName();

	void setInitScale(const D3DXVECTOR3& scale);
	void setInitQrientation(const D3DXQUATERNION& q);
	void setInitPosition(const D3DXVECTOR3& pos);

	void setAttachedBone(ElBone* bone);
	ElBone* getAttachedBone();

protected:
	virtual void updateFromParentImpl();

protected:
	ElBone*			mBone;
	std::string		mParentName;

	D3DXQUATERNION	mInitOrientation;
	D3DXVECTOR3		mInitPosition;
	D3DXVECTOR3		mInitScale;
};

#endif //__ElDummyObject_H__