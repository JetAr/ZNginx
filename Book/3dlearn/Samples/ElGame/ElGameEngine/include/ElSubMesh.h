#ifndef __ElSubMesh_H__
#define __ElSubMesh_H__

#include "ElD3DHeaders.h"
#include "ElMaterial.h"
#include "ElSubMeshRenderGroup.h"
#include "ElAnimation.h"
#include "ElDefines.h"
#include "ElSkeleton.h"
#include "ElSkeletalAnimation.h"
#include "ElAxisAlignedBox.h"
#include "ElArray.h"
#include <string>
#include <vector>

/**
 *
 * Mesh->SubMesh->MeshRenderGroup
 *
 */
class ElSubMesh
{
	friend class ElMeshSerializer;
	friend class ElSubEntity;

public:
	struct VertexData
	{
		D3DXVECTOR3			pos;
	};

	struct FaceData
	{
		int					v[3];
		int					mtl;
	};

	struct TVertexData
	{
		D3DXVECTOR2			tex;
	};

	struct TFaceData
	{
		int					tv[3];
	};

	struct NormalData
	{
		D3DXVECTOR3			fn;
		D3DXVECTOR3			vn[3];
	};

	struct SkinData
	{
		struct BoneWeightData
		{
			float			weight;
			std::string		boneName;
			int				boneID;		// not assigned until mesh linked to a skeleton
		};

		ElArray<BoneWeightData> bones[3];
	};

	typedef std::vector<ElSubMeshRenderGroup*> ElSubMeshRenderGroupList;

public:
	ElSubMesh();
	~ElSubMesh();

	void optimize();

	void setMtl(ElMaterial* mtl);
	void attachSkeleton(ElSkeleton* skeleton);
	void dettachSkeleton();

	const std::string& getName() const;
	bool hasSkeleton();

	void buildBounds();
	const ElAxisAlignedBox& getBounds() const;

protected:
	// rough data
	D3DXMATRIX					mInitTM;
	std::string					mName;
	std::string					mParentName;

	ElArray<VertexData>			mVertexData;
	ElArray<FaceData>			mFaceData;
	ElArray<TVertexData>		mTVertexData;
	ElArray<TFaceData>			mTFaceData;
	ElArray<NormalData>			mNormalData;
	ElArray<SkinData>			mSkinData;

	// optimized data
	ElMaterial*					mMaterial;
	ElSubMeshRenderGroupList	mRenderGroups;
	ElSkeleton*					mSkeleton;
	ElAxisAlignedBox			mAABB;
};

typedef std::vector<ElSubMesh*> ElSubMeshList;
typedef std::vector<ElSubMesh*>::iterator ElSubMeshIterator;

#endif //__ElSubMesh_H__