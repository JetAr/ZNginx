#include "ElApplicationPCH.h"
#include "ElSubEntity.h"
#include "ElDeviceManager.h"

ElSubEntity::ElSubEntity(ElSubMesh* subMesh)
: mSubMesh(subMesh)
, mpVertexBuffer(NULL)
, mAnimation(NULL)
, mBuffPtr(NULL)
, mFVF(0)
, mVertexSize(0)
, mNumVertices(0)
{

}

ElSubEntity::~ElSubEntity()
{
	ElSafeRelease(mpVertexBuffer);
}

bool ElSubEntity::initialise()
{
	// organize vertex data of sub meshes in to our own vertex structs
	int numVertex = mSubMesh->mFaceData.size() * 3;
	mVertices.assign(numVertex);
	for (int i = 0; i < mSubMesh->mFaceData.size(); ++i)
	{
		ElSubMesh::FaceData f = mSubMesh->mFaceData[i];
		ElSubMesh::TFaceData tf = mSubMesh->mTFaceData[i];
		ElSubMesh::NormalData n = mSubMesh->mNormalData[i];

		for (int j = 0; j < 3; ++j)
		{
			int idx = i * 3 + j;
			mVertices[idx].pos = mSubMesh->mVertexData[f.v[j]].pos;
			mVertices[idx].normal = n.vn[j];
			mVertices[idx].tex = mSubMesh->mTVertexData[tf.tv[j]].tex;
		}
	}

	mBuffPtr = (void*)mVertices.base();
	mFVF = VERTEX::FVF;
	mVertexSize = sizeof(VERTEX);
	mNumVertices = mVertices.size();

	// create d3d vertex buffer
	updateVertexBuffer(true);

	return true;
}

void ElSubEntity::updateVertexBuffer(bool forceCreate /* = false */)
{
	unsigned int buffSize = mNumVertices * mVertexSize;

	if (mpVertexBuffer == NULL || forceCreate)
	{
		ElSafeRelease(mpVertexBuffer);
		ElDeviceManager::getDevice()->CreateVertexBuffer(buffSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &mpVertexBuffer, NULL);
	}

	void* buff = NULL;
	mpVertexBuffer->Lock(0, buffSize, &buff, 0);
	memcpy(buff, mBuffPtr, buffSize);
	mpVertexBuffer->Unlock();
}

void ElSubEntity::update(const ElAnimationState& as)
{
	if (mAnimation)
		doMeshAnimation(as);

	// do not need to calculate by ourselves anymore, we notify the d3d to
	// do it(vertex blend) at the parent Entity part
	//if (mSubMesh->hasSkeleton() && mSubMesh->mSkeleton)
	//	doSkeletalAnimation();
}

void ElSubEntity::render(float alpha)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();
	if (SUCCEEDED(d3dDevice->BeginScene()))
	{
		d3dDevice->SetFVF(mFVF);
		d3dDevice->SetStreamSource(0, mpVertexBuffer, 0, mVertexSize);

		int numSub = mSubMesh->mRenderGroups.size();
		for (int i = 0; i < numSub; ++i)
		{
			// set material, do it here allows us to assign different material to
			// each sub entities
			ElMaterial* mtl = mSubMesh->mRenderGroups[i]->getMtl();
			if (mtl)
			{
				if (!float_equal(alpha, 1.0f))
				{
					D3DMATERIAL9 d3dMtl = *mtl->getD3DMaterial();
					d3dMtl.Diffuse.a *= alpha;
					d3dDevice->SetMaterial(&d3dMtl);
				}
				else
					d3dDevice->SetMaterial(mtl->getD3DMaterial());

				// set texture
				ElTexture* texture = mtl->getTexture();
				if (texture)
					d3dDevice->SetTexture(0, texture->getTexture());
				else
					d3dDevice->SetTexture(0, NULL);
			}

			mSubMesh->mRenderGroups[i]->render(mNumVertices);
		}

		d3dDevice->EndScene();
	}
}

ElSubMesh* ElSubEntity::getSubMesh()
{
	return mSubMesh;
}

void ElSubEntity::setAnimation(ElSubMeshAnimation* sma)
{
	mAnimation = sma;

	assert(mAnimation == NULL || mAnimation->getFrame(0)->normals.size() == mSubMesh->mNormalData.size());
	assert(mAnimation == NULL || mAnimation->getFrame(0)->verts.size() == mSubMesh->mVertexData.size());
}

void ElSubEntity::_notifySkeletonAttached(ElSkeleton* skeleton)
{
	if (mSubMesh->hasSkeleton() && mSubMesh->mSkeleton)
	{
		// organize vertex data of sub meshes in to our own blend vertex structs
		int numVertex = mSubMesh->mFaceData.size() * 3;
		mBlendVertices.assign(numVertex);
		for (int i = 0; i < mSubMesh->mFaceData.size(); ++i)
		{
			ElSubMesh::FaceData f = mSubMesh->mFaceData[i];
			ElSubMesh::TFaceData tf = mSubMesh->mTFaceData[i];
			ElSubMesh::NormalData n = mSubMesh->mNormalData[i];

			for (int j = 0; j < 3; ++j)
			{
				int idx = i * 3 + j;
				mBlendVertices[idx].pos = mSubMesh->mVertexData[f.v[j]].pos;
				mBlendVertices[idx].normal = n.vn[j];
				mBlendVertices[idx].tex = mSubMesh->mTVertexData[tf.tv[j]].tex;

				// blend weight and matrixIdx
				float blend[ElMaxVertexWeight];
				DWORD matrixIndices[ElMaxVertexWeight];
				ZeroMemory(blend, sizeof(blend));
				ZeroMemory(matrixIndices, sizeof(matrixIndices));

				int numBones = min(mSubMesh->mSkinData[i].bones[j].size(), ElMaxVertexWeight);
				for (int b = 0; b < numBones; ++b)
				{
					blend[b] = mSubMesh->mSkinData[i].bones[j][b].weight;
					matrixIndices[b] = mSubMesh->mSkinData[i].bones[j][b].boneID;
				}

				mBlendVertices[idx].weights[0] = blend[0];
				mBlendVertices[idx].weights[1] = blend[1];
				mBlendVertices[idx].weights[2] = blend[2];
				mBlendVertices[idx].matrixIndices = ElBlendMatrixIdx(matrixIndices[3], matrixIndices[2], matrixIndices[1], matrixIndices[0]);
			}
		}

		mBuffPtr = (void*)mBlendVertices.base();
		mFVF = BLENDVERTEX::FVF;
		mVertexSize = sizeof(BLENDVERTEX);
		mNumVertices = mBlendVertices.size();

		updateVertexBuffer(true);
	}
}

void ElSubEntity::doMeshAnimation(const ElAnimationState& as)
{
	int numFrames = mAnimation->getNumFrames();
	int startIdx = min(numFrames, as.startIdx);
	int endIdx = min(numFrames, as.endIdx);

	ElSubMeshAnimation::Frame* startFrame = mAnimation->getFrame(startIdx);
	ElSubMeshAnimation::Frame* endFrame = mAnimation->getFrame(endIdx);

	for (int i = 0; i < mSubMesh->mFaceData.size(); ++i)
	{
		ElSubMesh::FaceData f = mSubMesh->mFaceData[i];
		ElSubMesh::NormalData n = mSubMesh->mNormalData[i];

		for (int j = 0; j < 3; ++j)
		{
			int idx = i * 3 + j;

			D3DXVECTOR3 startPos = startFrame->verts[f.v[j]].pos;
			D3DXVECTOR3 endPos = endFrame->verts[f.v[j]].pos;
			mVertices[idx].pos = startPos + (endPos - startPos) * as.scale;

			D3DXVECTOR3 startNormal = startFrame->normals[i].vn[j];
			D3DXVECTOR3 endNormal = endFrame->normals[i].vn[j];
			mVertices[idx].normal = startNormal + (endNormal - startNormal) * as.scale;
		}
	}

	// recreate the d3d vertex buffer to update
	updateVertexBuffer();
}

void ElSubEntity::doSkeletalAnimation()
{
	D3DXMATRIX tm, m;
	int i, j, b;
	for (i = 0; i < mSubMesh->mFaceData.size(); ++i)
	{
		for (j = 0; j < 3; ++j)
		{
			if (mSubMesh->mSkinData[i].bones[j].size() == 0)
			{
				// if the vertex has not been banded to any bones,
				// we just assign an identity matrix
				D3DXMatrixIdentity(&tm);
			}
			else
			{
				// else, the vertex will be transformed by using a bone-weighted matrix
				ZeroMemory(&tm, sizeof(tm));
				for (b = 0; b < mSubMesh->mSkinData[i].bones[j].size(); ++b)
				{
					m = mSubMesh->mSkeleton->getBone(mSubMesh->mSkinData[i].bones[j][b].boneID)->mTransform * mSubMesh->mSkinData[i].bones[j][b].weight;
					tm += m;
				}
			}

			int idx = i * 3 + j;
			D3DXVec3TransformCoord(&mVertices[idx].pos, &mSubMesh->mVertexData[mSubMesh->mFaceData[i].v[j]].pos, &tm);
			D3DXVec3TransformCoord(&mVertices[idx].normal, &mSubMesh->mNormalData[i].vn[j], &tm);
		}
	}

	// recreate the d3d vertex buffer to update
	updateVertexBuffer();
}


