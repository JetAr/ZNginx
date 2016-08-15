#ifndef __ElMesh_H__
#define __ElMesh_H__

#include "ElSubMesh.h"
#include "ElAxisAlignedBox.h"
#include "ElMeshAnimation.h"
#include <string>

class ElHelperObject
{
public:
	ElHelperObject();
	virtual ~ElHelperObject();

public:
	std::string		mName;
	std::string		mParentName;
	std::string		mClassName;
	
	D3DXQUATERNION	mOrientation;
	D3DXVECTOR3		mPosition;
	D3DXVECTOR3		mScale;
	
	D3DXVECTOR3		mBoundingBoxMin;
	D3DXVECTOR3		mBoundingBoxMax;
};
typedef std::vector<ElHelperObject*> ElHelperObjectList;
typedef std::vector<ElHelperObject*>::iterator ElHelperObjectIterator;

class ElMesh
{
public:
	typedef std::map<std::string, ElMeshAnimation*> AnimationMap;
	typedef AnimationMap::iterator AnimationIterator;
	typedef AnimationMap::const_iterator AnimationConstIterator;
	typedef AnimationMap::value_type AnimationValue;

public:
	ElMesh();
	virtual ~ElMesh();

	bool		load(LPCSTR filename);

	void		setSubMeshList(ElSubMeshList& list);
	void		setHelperObjList(ElHelperObjectList& list);
	
	void		attachSkeleton(ElSkeleton* skeleton);
	void		dettachSkeleton();

	const std::string& getName();
	int			getNumSubMeshes();
	ElSubMesh*	getSubMesh(int idx);
	ElSubMesh*	getSubMesh(const std::string name);
	int			getNumHelperObjects();
	ElHelperObject*	getHelperObject(int idx);

	bool		hasMeshAnimation();
	bool		createAnimation(LPCSTR filename, const std::string& name);
	ElMeshAnimation* getAnimation(const std::string& name);

	bool		hasSkeleton();
	ElSkeleton*	getSkeleton();

	void		buildBounds();
	const ElAxisAlignedBox& getBounds() const;

protected:
	std::string			mName;
	ElSubMeshList		mSubMeshList;
	ElHelperObjectList	mHelperObjList;
	AnimationMap		mAnimationList;

	ElSkeleton*			mSkeleton;

	ElAxisAlignedBox	mAABB;
};

#endif //__ElMesh_H__