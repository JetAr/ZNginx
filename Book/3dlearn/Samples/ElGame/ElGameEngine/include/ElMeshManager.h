#ifndef __ElMeshManager_H__
#define __ElMeshManager_H__

#include "ElSingleton.h"
#include "ElMesh.h"
#include <map>
#include <string>

class ElMeshManager
{
	ElDeclareSingleton(ElMeshManager);

public:
	typedef std::map<std::string, ElMesh*> MeshMap;
	typedef MeshMap::iterator MeshIterator;
	typedef MeshMap::const_iterator MeshConstIterator;
	typedef MeshMap::value_type MeshValue;

	ElMeshManager();
	virtual ~ElMeshManager();

	ElMesh*		createMesh(LPCSTR filename);
	ElMesh*		getMesh(const std::string& name);

protected:
	MeshMap		mMeshList;
};

#endif //__ElMeshManager_H__