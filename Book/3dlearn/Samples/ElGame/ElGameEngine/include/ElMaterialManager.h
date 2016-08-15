#ifndef __ElMaterialManager_H__
#define __ElMaterialManager_H__

#include "ElD3DHeaders.h"
#include "ElMaterial.h"
#include "ElSingleton.h"
#include <vector>

class ElMaterialManager
{
	ElDeclareSingleton(ElMaterialManager);

public:
	ElMaterialManager();
	virtual ~ElMaterialManager();

	void addMtl(ElMaterial* mtl);
	void addMtlList(ElMaterialList& list);

protected:
	ElMaterialList	mMtlList;
};

#endif //__ElMaterialManager_H__