#include "ElApplicationPCH.h"
#include "ElMaterialManager.h"
#include "ElDefines.h"

ElImplementSingleton(ElMaterialManager);

ElMaterialManager::ElMaterialManager()
{

}

ElMaterialManager::~ElMaterialManager()
{
	for (ElMaterialIterator iter = mMtlList.begin(); iter != mMtlList.end(); ++iter)
	{
		ElSafeDelete(*iter);
	}
}

void ElMaterialManager::addMtl(ElMaterial* mtl)
{
	mMtlList.push_back(mtl);
}

void ElMaterialManager::addMtlList(ElMaterialList& list)
{
	mMtlList.insert(mMtlList.end(), list.begin(), list.end());
}