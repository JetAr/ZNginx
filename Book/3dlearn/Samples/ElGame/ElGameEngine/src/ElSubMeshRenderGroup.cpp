#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElSubMeshRenderGroup.h"
#include "ElTexture.h"
#include "ElDeviceManager.h"

ElSubMeshRenderGroup::ElSubMeshRenderGroup()
: mMaterial(NULL)
, mIndexBuffer(NULL)
{

}

ElSubMeshRenderGroup::~ElSubMeshRenderGroup()
{
	ElSafeRelease(mIndexBuffer);
}

void ElSubMeshRenderGroup::setMtl(ElMaterial* mtl)
{
	mMaterial = mtl;
}

ElMaterial* ElSubMeshRenderGroup::getMtl() const
{
	return mMaterial;
}

void ElSubMeshRenderGroup::addVertexIndex(DWORD idx)
{
	mVIList.push_back(idx);
}

void ElSubMeshRenderGroup::optimize()
{
	if (mVIList.empty())
		return;

	int numIndex = mVIList.size();
	mIndices.assign(numIndex);
	for (int i = 0; i < numIndex; ++i)
		mIndices[i] = mVIList[i];
	mVIList.clear();

	// update d3d index buffer
	updateIndexBuffer();
}

void ElSubMeshRenderGroup::render(int numVertex)
{
	IDirect3DDevice9* d3dDevice = ElDeviceManager::getDevice();

	// set d3d index buffer
	d3dDevice->SetIndices(mIndexBuffer);

	d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertex, 0, mIndices.size() / 3);

	d3dDevice->SetTexture(0, NULL);
}

void ElSubMeshRenderGroup::updateIndexBuffer(bool forceCreate /* = false */)
{
	unsigned int buffSize = mIndices.size() * sizeof(DWORD);

	if (mIndexBuffer == NULL || forceCreate)
		ElDeviceManager::getDevice()->CreateIndexBuffer(buffSize, D3DUSAGE_WRITEONLY, D3DFMT_INDEX32, D3DPOOL_MANAGED, &mIndexBuffer, NULL);

	void* buff = NULL;
	mIndexBuffer->Lock(0, buffSize, &buff, 0);
	memcpy(buff, mIndices.base(), buffSize);
	mIndexBuffer->Unlock();
}