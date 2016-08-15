#include "ElApplicationPCH.h"
#include "ElSubMesh.h"

ElSubMesh::ElSubMesh()
: mName("")
, mParentName("")
, mMaterial(NULL)
, mSkeleton(NULL)
{
	D3DXMatrixIdentity(&mInitTM);
}

ElSubMesh::~ElSubMesh()
{
	int numSub = mRenderGroups.size();
	for (int i = 0; i < numSub; ++i)
	{
		ElSafeDelete(mRenderGroups[i]);
	}
	mRenderGroups.clear();
}

void ElSubMesh::optimize()
{
	// construct sub meshes according to sub materials
	bool hasSub = false;
	int numSub = 1;

	if (mMaterial)
	{
		hasSub = mMaterial->hasSubMtl();
		numSub = max(1, mMaterial->getSubNum());
	}

	if (!hasSub)
	{
		ElSubMeshRenderGroup* rg = ElNew ElSubMeshRenderGroup;
		rg->setMtl(mMaterial);
		mRenderGroups.push_back(rg);
	}
	else
	{
		for (int i = 0; i < numSub; ++i)
		{
			ElSubMeshRenderGroup* rg = ElNew ElSubMeshRenderGroup;
			rg->setMtl(mMaterial->getSubMtl(i));
			mRenderGroups.push_back(rg);
		}
	}

	// organize vertex into sub meshes according to the mtl ID
	for (int i = 0; i < mFaceData.size(); ++i)
	{
		FaceData f = mFaceData[i];
		TFaceData tf = mTFaceData[i];
		NormalData n = mNormalData[i];

		int subID = hasSub ? f.mtl : 0;
		ElSubMeshRenderGroup* rg = mRenderGroups[subID];

		for (int j = 0; j < 3; ++j)
		{
			int idx = i * 3 + j;
			rg->addVertexIndex(idx);
		}
	}
	
	// optimize each sub meshes
	for (int i = 0; i < numSub; ++i)
		mRenderGroups[i]->optimize();

	// build the bounding box
	buildBounds();
}

void ElSubMesh::setMtl(ElMaterial* mtl)
{
	mMaterial = mtl;
}

void ElSubMesh::attachSkeleton(ElSkeleton* skeleton)
{
	mSkeleton = skeleton;

	for (int f = 0; f < mFaceData.size(); ++f)
	{
		SkinData& s = mSkinData[f];
		for (int i = 0; i < 3; ++i)
		{
			for (int b = 0; b < s.bones[i].size(); ++b)
			{
				s.bones[i][b].boneID = mSkeleton->getBoneID(s.bones[i][b].boneName);
				assert(s.bones[i][b].boneID != -1);
			}
		}
	}
}

void ElSubMesh::dettachSkeleton()
{
	mSkeleton = NULL;
}

const std::string& ElSubMesh::getName() const
{
	return mName;
}

bool ElSubMesh::hasSkeleton()
{
	return !mSkinData.empty();
}

void ElSubMesh::buildBounds()
{
	for (int i = 0; i < mVertexData.size(); ++i)
		mAABB.merge(mVertexData[i].pos);
}

const ElAxisAlignedBox& ElSubMesh::getBounds() const
{
	return mAABB;
}

