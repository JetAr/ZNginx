#ifndef __ElSubMeshRenderGroup_H__
#define __ElSubMeshRenderGroup_H__

#include "ElD3DHeaders.h"
#include "ElMaterial.h"
#include "ElArray.h"

class ElSubMeshRenderGroup
{
	typedef std::vector<DWORD> SubMeshVIList;

public:
	ElSubMeshRenderGroup();
	virtual ~ElSubMeshRenderGroup();

	void setMtl(ElMaterial* mtl);
	ElMaterial* getMtl() const;
	void addVertexIndex(DWORD idx);

	void optimize();
	void render(int numVertex);

	void updateIndexBuffer(bool forceCreate = false);

protected:
	ElMaterial*				mMaterial;
	ElArray<DWORD>			mIndices;
	IDirect3DIndexBuffer9*	mIndexBuffer;

	SubMeshVIList			mVIList;
};

#endif //__ElSubMeshRenderGroup_H__