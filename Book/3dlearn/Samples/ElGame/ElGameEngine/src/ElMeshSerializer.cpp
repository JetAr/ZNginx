#include "ElApplicationPCH.h"
#include "ElDefines.h"
#include "ElMeshSerializer.h"
#include "ElTerrain.h"
#include "ElPath.h"
#include "ElMaterialManager.h"

ElMeshSerializer::ElMeshSerializer()
{

}

ElMeshSerializer::~ElMeshSerializer()
{

}

bool ElMeshSerializer::loadMesh(LPCSTR filename, ElMesh* mesh)
{
	// clear cache if there is any
	mCachedMtlList.clear();
	mCachedSubMeshList.clear();
	mCachedHelperObjList.clear();

	// open stream
	infile.open(filename);
	if (!infile.good())
		return false;

	// record the directory
	dir = ElPath::getDirFromPath(filename);

	// import the mesh
	infile >> token;
	while (!infile.eof())
	{
		if (token.compare("ElMaxPlugin") == 0)
		{
			importGlobalInfo();
		}
		else if (token.compare("MaterialList") == 0)
		{
			importMaterialList();
		}
		else if (token.compare("GeomObject") == 0)
		{
			importGeomObject();
		}
		else if (token.compare("HelperObject") == 0)
		{
			importHelperObject();
		}
		else
		{
			// something is wrong
			break;
		}
	}

	// close stream
	infile.close();
	infile.clear();

	// optimize meshes
	int numMesh = mCachedSubMeshList.size();
	for (int i = 0; i < numMesh; ++i)
	{
		mCachedSubMeshList[i]->optimize();
	}

	// set subMeshes, helperObjects refs from cached list to mesh,
	// and then release them
	mesh->setSubMeshList(mCachedSubMeshList);
	mCachedSubMeshList.clear();

	mesh->setHelperObjList(mCachedHelperObjList);
	mCachedHelperObjList.clear();

	// optimize self, primarily, add the material refs into
	// material manager, and then release them
	ElMaterialManager::getSingleton().addMtlList(mCachedMtlList);
	mCachedMtlList.clear();

	return true;
}

bool ElMeshSerializer::loadMeshAnimation(LPCSTR filename, ElMeshAnimation* meshAnim)
{
	// open stream
	infile.open(filename);
	if (!infile.good())
		return false;

	// import the meshAnimation
	infile >> token;
	while (!infile.eof())
	{
		if (token.compare("ElMaxPlugin") == 0)
		{
			importGlobalInfo();
		}
		else if (token.compare("GeomObject") == 0)
		{
			importGeomObjectAnim(meshAnim);
		}
		else
		{
			// something is wrong
			break;
		}
	}

	// close stream
	infile.close();
	infile.clear();

	return true;
}

void ElMeshSerializer::importGlobalInfo()
{
	infile >> token;
	if (token.compare("Version") == 0)
	{
		int version;
		infile >> version;
		infile >> token;
	}
}

void ElMeshSerializer::importMaterialList()
{
	int numMtl;
	infile >> numMtl;

	for (int i = 0; i < numMtl; ++i)
	{
		ElMaterial* mtl = ElNew ElMaterial;
		mCachedMtlList.push_back(mtl);
	}

	infile >> token;
	for (int i = 0; i < numMtl; ++i)
	{
		if (token.compare("Material") == 0)
			importMaterial(mCachedMtlList[i]);
	}
}

void ElMeshSerializer::importMaterial(ElMaterial* mtl)
{
	int id;
	infile >> id;

	infile >> token;
	if (token.compare("MaterialName") == 0)
	{
		char name[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(name, sizeof(name));
	}
	
	D3DCOLORVALUE color;
	color.a = 1.0f;
	infile >> color.r >> color.g >> color.b;
	mtl->setAmbient(color);
	infile >> color.r >> color.g >> color.b;
	mtl->setDiffuse(color);
	infile >> color.r >> color.g >> color.b;
	mtl->setSpecular(color);

	infile >> token;
	if (token.compare("Bitmap") == 0)
	{
		char texture[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(texture, sizeof(texture));

		// get the filename, if not exists, merge it with the
		// directory path where the model is
		std::string filename;
		if (ElPath::isFileExist(texture))
		{
			filename = texture;
		}
		else
		{
			filename = ElPath::getFileNameFromPath(texture);
			filename = dir + filename;
		}

		mtl->setTexture(filename.c_str());

		infile >> token;
	}

	if (token.compare("NumSubMtls") == 0)
	{
		int numSub;
		infile >> numSub;

		for (int i = 0; i < numSub; ++i)
		{
			ElMaterial* subMtl = ElNew ElMaterial;
			mtl->addSubMtl(subMtl);
		}

		infile >> token;
		for (int i = 0; i < numSub; ++i)
		{
			if (token.compare("SubMaterial") == 0)
				importMaterial(mtl->getSubMtl(i));
		}
	}
}

void ElMeshSerializer::importGeomObject()
{
	ElSubMesh* subMesh = ElNew ElSubMesh;
	mCachedSubMeshList.push_back(subMesh);

	infile >> token;
	if (token.compare("NodeName") == 0)
	{
		char name[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(name, sizeof(name));
		subMesh->mName = name;

		infile >> token;
	}

	if (token.compare("NodeParent") == 0)
	{
		char parent[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(parent, sizeof(parent));
		subMesh->mParentName = parent;

		infile >> token;
	}

	if (token.compare("NodeTM") == 0)
	{
		infile >> subMesh->mInitTM._11 >> subMesh->mInitTM._13 >> subMesh->mInitTM._12
			>> subMesh->mInitTM._31 >> subMesh->mInitTM._33 >> subMesh->mInitTM._32
			>> subMesh->mInitTM._21 >> subMesh->mInitTM._23 >> subMesh->mInitTM._22
			>> subMesh->mInitTM._41 >> subMesh->mInitTM._43 >> subMesh->mInitTM._42;

		infile >> token;
	}

	if (token.compare("MaterialRef") == 0)
	{
		int mtlID;
		infile >> mtlID;
		if (mtlID >= 0)
			subMesh->setMtl(mCachedMtlList[mtlID]);

		infile >> token;
	}

	if (token.compare("Mesh") == 0)
		importMesh(subMesh);
}

void ElMeshSerializer::importMesh(ElSubMesh* subMesh)
{
	infile >> token;
	if (token.compare("TimeValue") == 0)
	{
		int time;
		infile >> time;

		infile >> token;
	}

	if (token.compare("VertexList") == 0)
	{
		// import vertex data
		int numVertex;
		infile >> numVertex;
		subMesh->mVertexData.assign(numVertex);

		for (int i = 0; i < numVertex; ++i)
			infile >> subMesh->mVertexData[i].pos.x >> subMesh->mVertexData[i].pos.z >> subMesh->mVertexData[i].pos.y;

		infile >> token;
	}

	if (token.compare("FaceList") == 0)
	{
		// import faces data
		int numFaces;
		infile >> numFaces;
		subMesh->mFaceData.assign(numFaces);

		for (int i = 0; i < numFaces; ++i)
			infile >> subMesh->mFaceData[i].v[0] >> subMesh->mFaceData[i].v[2] >> subMesh->mFaceData[i].v[1] >> subMesh->mFaceData[i].mtl;

		infile >> token;
	}

	if (token.compare("TVertexList") == 0)
	{
		// import vertex texcoords data
		int numTVertex;
		infile >> numTVertex;
		subMesh->mTVertexData.assign(numTVertex);

		float tmp;
		for (int i = 0; i < numTVertex; ++i)
		{
			infile >> subMesh->mTVertexData[i].tex.x >> subMesh->mTVertexData[i].tex.y >> tmp;
			subMesh->mTVertexData[i].tex.y = 1.0f - subMesh->mTVertexData[i].tex.y;
		}

		infile >> token;
	}

	if (token.compare("TFaceList") == 0)
	{
		// import face texcoords data
		int numTFaces;
		infile >> numTFaces;
		subMesh->mTFaceData.assign(numTFaces);

		for (int i = 0; i < numTFaces; ++i)
			infile >> subMesh->mTFaceData[i].tv[0] >> subMesh->mTFaceData[i].tv[2] >> subMesh->mTFaceData[i].tv[1];

		infile >> token;
	}

	if (token.compare("NormalList") == 0)
	{
		// import normal data
		int numNormals;
		infile >> numNormals;
		subMesh->mNormalData.assign(numNormals);

		int f, v;
		for (int i = 0; i < numNormals; ++i)
		{
			infile >> f >> subMesh->mNormalData[i].fn.x >> subMesh->mNormalData[i].fn.y >> subMesh->mNormalData[i].fn.z
				>> v >> subMesh->mNormalData[i].vn[0].x >> subMesh->mNormalData[i].vn[0].z >> subMesh->mNormalData[i].vn[0].y
				>> v >> subMesh->mNormalData[i].vn[2].x >> subMesh->mNormalData[i].vn[2].z >> subMesh->mNormalData[i].vn[2].y
				>> v >> subMesh->mNormalData[i].vn[1].x >> subMesh->mNormalData[i].vn[1].z >> subMesh->mNormalData[i].vn[1].y;
		}

		infile >> token;
	}

	if (token.compare("SkinInfoList") == 0)
	{
		// import skin data
		int numFaces;
		infile >> numFaces;
		assert(numFaces == subMesh->mFaceData.size());
		subMesh->mSkinData.assign(numFaces);

		int f;
		for (int i = 0; i < numFaces; ++i)
		{
			infile >> token >> f;
			assert(token.compare("Face") == 0);
			assert(f == i);

			importSkin(subMesh, i, 0);
			importSkin(subMesh, i, 2);
			importSkin(subMesh, i, 1);
		}

		infile >> token;
	}
}

void ElMeshSerializer::importSkin(ElSubMesh* subMesh, int face, int idx)
{
	int vert, numBones;
	infile >> token >> vert >> numBones;
	assert(token.compare("Vertex") == 0);
	subMesh->mSkinData[face].bones[idx].assign(numBones);

	char boneName[EL_MAX_PATH] = {0};
	for (int i = 0; i < subMesh->mSkinData[face].bones[idx].size(); ++i)
	{
		infile >> subMesh->mSkinData[face].bones[idx][i].weight;
		infile.get();
		infile.getline(boneName, sizeof(boneName));
		subMesh->mSkinData[face].bones[idx][i].boneName = boneName;
	}
}

void ElMeshSerializer::importHelperObject()
{
	ElHelperObject* helperObj = ElNew ElHelperObject;
	mCachedHelperObjList.push_back(helperObj);

	infile >> token;
	if (token.compare("NodeName") == 0)
	{
		char name[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(name, sizeof(name));
		helperObj->mName = name;

		infile >> token;
	}

	if (token.compare("NodeParent") == 0)
	{
		char parent[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(parent, sizeof(parent));
		helperObj->mParentName = parent;

		infile >> token;
	}
	
	if (token.compare("HelperClass") == 0)
	{
		char classname[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(classname, sizeof(classname));
		helperObj->mClassName = classname;

		infile >> token;
	}

	if (token.compare("NodeTM") == 0)
	{
		D3DXMATRIX m;
		infile >> m._11 >> m._13 >> m._12
			>> m._31 >> m._33 >> m._32
			>> m._21 >> m._23 >> m._22
			>> m._41 >> m._43 >> m._42;
		D3DXMatrixDecompose(&helperObj->mScale, &helperObj->mOrientation, &helperObj->mPosition, &m);

		infile >> token;
	}

	if (token.compare("BoundingBoxMin") == 0)
	{
		infile >> helperObj->mBoundingBoxMin.x >> helperObj->mBoundingBoxMin.z >> helperObj->mBoundingBoxMin.y;
		infile >> token;
	}

	if (token.compare("BoundingBoxMax") == 0)
	{
		infile >> helperObj->mBoundingBoxMax.x >> helperObj->mBoundingBoxMax.z >> helperObj->mBoundingBoxMax.y;
		infile >> token;
	}
}

void ElMeshSerializer::importGeomObjectAnim(ElMeshAnimation* meshAnim)
{
	std::string subMeshName("");
	ElSubMeshAnimation* subMeshAnim = ElNew ElSubMeshAnimation;

	infile >> token;
	if (token.compare("NodeName") == 0)
	{
		char name[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(name, sizeof(name));
		subMeshName = name;

		infile >> token;
	}

	if (token.compare("MeshAnimation") == 0)
		importMeshAnimation(subMeshAnim);

	meshAnim->addAnimation(subMeshName, subMeshAnim);
}

void ElMeshSerializer::importMeshAnimation(ElSubMeshAnimation* subMeshAnim)
{
	infile >> token;

	while (!infile.eof() && token.compare("Frame") == 0)
		importMeshFrame(subMeshAnim);
}

void ElMeshSerializer::importMeshFrame(ElSubMeshAnimation* subMeshAnim)
{
	ElSubMeshAnimation::Frame* frame = ElNew ElSubMeshAnimation::Frame;

	infile >> token;
	if (token.compare("TimeValue") == 0)
	{
		infile >> frame->t;

		infile >> token;
	}

	if (token.compare("VertexList") == 0)
	{
		// import frame vertex data
		int numVertex;
		infile >> numVertex;
		frame->verts.assign(numVertex);

		for (int i = 0; i < numVertex; ++i)
			infile >> frame->verts[i].pos.x >> frame->verts[i].pos.z >> frame->verts[i].pos.y;

		infile >> token;
	}

	if (token.compare("NormalList") == 0)
	{
		// import frame normal data
		int numNormals;
		infile >> numNormals;
		frame->normals.assign(numNormals);

		int f, v;
		for (int i = 0; i < numNormals; ++i)
		{
			infile >> f >> frame->normals[i].fn.x >> frame->normals[i].fn.y >> frame->normals[i].fn.z
				>> v >> frame->normals[i].vn[0].x >> frame->normals[i].vn[0].z >> frame->normals[i].vn[0].y
				>> v >> frame->normals[i].vn[2].x >> frame->normals[i].vn[2].z >> frame->normals[i].vn[2].y
				>> v >> frame->normals[i].vn[1].x >> frame->normals[i].vn[1].z >> frame->normals[i].vn[1].y;
		}

		infile >> token;
	}

	subMeshAnim->addFrame(frame);
}



