#include "ElApplicationPCH.h"
#include "ElMeshManager.h"

ElImplementSingleton(ElMeshManager);

ElMeshManager::ElMeshManager()
{

}

ElMeshManager::~ElMeshManager()
{
	MeshIterator i;
	for (i = mMeshList.begin(); i != mMeshList.end(); ++i)
	{
		ElSafeDelete(i->second);
	}
	mMeshList.clear();
}

ElMesh* ElMeshManager::createMesh(LPCSTR filename)
{
	ElMesh* mesh = getMesh(filename);
	if (mesh == NULL)
	{
		mesh = ElNew ElMesh;
		if (!mesh->load(filename))
		{
			ElSafeDelete(mesh);
		}
		else
			mMeshList.insert(MeshValue(mesh->getName(), mesh));
	}

	return mesh;
}

ElMesh* ElMeshManager::getMesh(const std::string& name)
{
	MeshIterator i = mMeshList.find(name);
	if (i != mMeshList.end())
		return i->second;
	else
		return NULL;
}