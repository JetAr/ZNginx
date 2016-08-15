#ifndef __ElMeshSerializer_H__
#define __ElMeshSerializer_H__

#include "ElMaterial.h"
#include "ElMesh.h"
#include "ElSubMesh.h"
#include "ElSkeleton.h"
#include "ElMeshAnimation.h"
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

class ElTerrain;

class ElMeshSerializer
{
public:
	ElMeshSerializer();
	virtual ~ElMeshSerializer();

	bool loadMesh(LPCSTR filename, ElMesh* mesh);
	bool loadMeshAnimation(LPCSTR filename, ElMeshAnimation* meshAnim);

protected:
	void importGlobalInfo();
	void importMaterialList();
	void importMaterial(ElMaterial* mtl);
	void importGeomObject();
	void importMesh(ElSubMesh* subMesh);
	void importSkin(ElSubMesh* subMesh, int face, int idx);
	void importHelperObject();

	void importGeomObjectAnim(ElMeshAnimation* meshAnim);
	void importMeshAnimation(ElSubMeshAnimation* subMeshAnim);
	void importMeshFrame(ElSubMeshAnimation* subMeshAnim);

protected:
	ElMaterialList		mCachedMtlList;
	ElSubMeshList		mCachedSubMeshList;
	ElHelperObjectList	mCachedHelperObjList;
	
	std::ifstream		infile;
	std::string			token;
	std::string			dir;
};

#endif //__ElMeshSerializer_H__