#include "ElMaxPlugin.h"

void ElMaxPlugin::nodeEnum(INode* node)
{
	// The ObjectState is a 'thing' that flows down the pipeline containing
	// all information about the object. By calling EvalWorldState() we tell
	// max to eveluate the object at end of the pipeline.
	ObjectState os = node->EvalWorldState(0);

	// The obj member of ObjectState is the actual object we will export.
	if (os.obj)
	{
		// We look at the super class ID to determine the type of the object.
		SClass_ID sid = os.obj->SuperClassID();
		switch (sid)
		{
		case GEOMOBJECT_CLASS_ID:
			ExportGeomObject(node);
			break;
		case CAMERA_CLASS_ID:
			break;
		case LIGHT_CLASS_ID:
			break;
		case SHAPE_CLASS_ID:
			break;
		case HELPER_CLASS_ID:
			ExportHelperObject(node);
			break;
		}
	}

	int count = node->NumberOfChildren();
	for (int i = 0; i < count; ++i)
	{
		nodeEnum(node->GetChildNode(i));
	}
}

void ElMaxPlugin::FindMaterials(INode* node)
{
	// Add the nodes material to out material list
	// Null entries are ignored when added...
	mtlList.Add(node->GetMtl());

	// For each child of this node, we recurse into ourselves 
	// and increment the counter until no more children are found.
	for (int i = 0; i < node->NumberOfChildren(); ++i)
	{
		FindMaterials(node->GetChildNode(i));
	}
}

void ElMaxPlugin::ExportMaterialList()
{
	int numMtls = mtlList.Count();
	outfile << indent.str() << "MaterialList " << numMtls << endl;

	++indent;
	for (int i = 0; i < numMtls; ++i)
	{
		DumpMaterial(mtlList.Get(i), i, -1);
	}
	--indent;
}

void ElMaxPlugin::ExportGeomObject(INode* node)
{
	ObjectState os = node->EvalWorldState(GetStaticFrame());
	if (!os.obj)
		return;

	// Targets are actually geomobjects, but we will export them
	// from the camera and light objects, so we skip them here.
	if (os.obj->ClassID() == Class_ID(TARGET_CLASS_ID, 0))
		return;

	// If this node has been added into the bonesList, it will be
	// treated as a bone, and will not be exported here
	if (boneList.GetID(node) != -1)
		return;

	// Export node type
	outfile << indent.str() << "GeomObject" << endl;
	++indent;

	// Export node header
	ExportNodeHeader(node);

	// Export node TM
	ExportNodeTM(node);

	// Export Material
	ExportMaterial(node);

	// Export Mesh
	ExportMesh(node, 0);
	
	--indent;
}

void ElMaxPlugin::ExportNodeHeader(INode* node)
{
	// Node name
	outfile << indent.str() << "NodeName " << node->GetName() << endl;

	//  If the node is linked, export parent node name
	INode* parent = node->GetParentNode();
	if (parent && !parent->IsRootNode())
		outfile << indent.str() << "NodeParent " << parent->GetName() << endl;
}

void ElMaxPlugin::ExportNodeTM(INode* node)
{
	outfile << indent.str() << "NodeTM" << endl;
	++indent;

	// Dump the full matrix
	Matrix3 pivot = node->GetNodeTM(0);
	DumpMatrix3(&pivot);

	--indent;
}

void ElMaxPlugin::ExportMaterial(INode* node)
{
	int mtlID = -1;

	Mtl* mtl = node->GetMtl();
	if (mtl)
		mtlID = mtlList.GetID(mtl);
	else
		mtlID = -1;

	outfile << indent.str() << "MaterialRef " << mtlID << endl;
}

void ElMaxPlugin::ExportMesh(INode* node, TimeValue t)
{
	int i = 0;
	Mtl* nodeMtl = node->GetMtl();
	Matrix3 tm = node->GetObjTMAfterWSM(t);
	BOOL negScale = TMNegParity(tm);
	int vx1, vx2, vx3;

	ObjectState os = node->EvalWorldState(t);
	if (!os.obj || os.obj->SuperClassID() != GEOMOBJECT_CLASS_ID)
		return; // Safety net. This shouldn't happen.

	// Order of the vertices. Get 'em counter clockwise if the objects is
	// negatively scaled.
	if (negScale)
	{
		vx1 = 2;
		vx2 = 1;
		vx3 = 0;
	}
	else
	{
		vx1 = 0;
		vx2 = 1;
		vx3 = 2;
	}

	BOOL needDel;
	TriObject* tri = GetTriObjectFromNode(node, t, needDel);
	if (!tri)
		return;

	Mesh* mesh = &tri->GetMesh();

	mesh->buildNormals();

	// Export mesh info
	outfile << indent.str() << "Mesh" << endl;
	++indent;
	outfile << indent.str() << "TimeValue " << t << endl;

	// Export the vertices
	int numVertex = mesh->getNumVerts();
	outfile << indent.str() << "VertexList " << numVertex << endl;
	++indent;
	for (i = 0; i < numVertex; ++i)
	{
		Point3 v = tm * mesh->verts[i];
		outfile << indent.str() << v.x << ' ' << v.y << ' ' << v.z << endl;
	}
	--indent;

	// Export faces
	int numFaces = mesh->getNumFaces();
	outfile << indent.str() << "FaceList " << numFaces << endl;
	++indent;
	for (i = 0; i < numFaces; ++i)
	{
		outfile << indent.str() <<
			mesh->faces[i].v[vx1] << ' ' <<
			mesh->faces[i].v[vx2] << ' ' <<
			mesh->faces[i].v[vx3] << ' ' <<
			mesh->faces[i].getMatID() << endl;
	}
	--indent;

	// Export face map texcoords if we have them...
	int numTVx = mesh->getNumTVerts();
	outfile << indent.str() << "TVertexList " << numTVx << endl;

	if (numTVx)
	{
		++indent;
		for (i = 0; i < numTVx; ++i)
		{
			UVVert tv = mesh->tVerts[i];
			outfile << indent.str() << clampNan(tv.x) << ' ' << clampNan(tv.y) << ' ' << clampNan(tv.z) << endl;
		}
		--indent;

		outfile << indent.str() << "TFaceList " << numFaces << endl;
		++indent;
		for (i = 0; i < numFaces; ++i)
		{
			outfile << indent.str() << mesh->tvFace[i].t[vx1] << ' '
				<< mesh->tvFace[i].t[vx2] << ' '
				<< mesh->tvFace[i].t[vx3] << endl;
		}
		--indent;
	}

	// Export mesh (face + vertex) normals
	outfile << indent.str() << "NormalList " << numFaces << endl;
	++indent;

	Point3 fn;  // Face normal
	Point3 vn;  // Vertex normal
	int vert;
	Face* f;

	// Face and vertex normals.
	// In MAX a vertex can have more than one normal (but doesn't always have it).
	// This is depending on the face you are accessing the vertex through.
	// To get all information we need to export all three vertex normals
	// for every face.
	for (i = 0; i < numFaces; ++i)
	{
		f = &mesh->faces[i];
		fn = mesh->getFaceNormal(i);
		outfile << indent.str() << i << ' ' << fn.x << ' ' << fn.y << ' ' << fn.z << endl;

		++ indent;

		vert = f->getVert(vx1);
		vn = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
		outfile << indent.str() << vert << ' ' << vn.x << ' ' << vn.y << ' ' << vn.z << endl;

		vert = f->getVert(vx2);
		vn = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
		outfile << indent.str() << vert << ' ' << vn.x << ' ' << vn.y << ' ' << vn.z << endl;

		vert = f->getVert(vx3);
		vn = GetVertexNormal(mesh, i, mesh->getRVertPtr(vert));
		outfile << indent.str() << vert << ' ' << vn.x << ' ' << vn.y << ' ' << vn.z << endl;

		-- indent;
	}
	--indent;

	// Export skin information, if there is any
	ISkin* skin = FindSkinModifier(node);
	if (skin)
	{
		outfile << indent.str() << "SkinInfoList " << numFaces << endl;
		++indent;

		for (i = 0; i < numFaces; ++i)
		{
			f = &mesh->faces[i];
			outfile << indent.str() << "Face " << i << endl;
			++indent;

			for (int j = 0; j < 3; ++j)
			{
				vert = f->getVert(j);
				ISkinContextData* skinContex = skin->GetContextInterface(node);
				int numBones = skinContex->GetNumAssignedBones(vert);
				outfile << indent.str() << "Vertex " << vert << ' ' << numBones << endl;
				++indent;

				for (int b = 0; b < numBones; ++b)
				{
					INode* bone = skin->GetBone(skinContex->GetAssignedBone(vert, b));
					float weight = skinContex->GetBoneWeight(vert, b);
					assert(boneList.GetID(bone) != -1);
					outfile << indent.str() << weight << ' ' << bone->GetName() << endl;
				}

				--indent;
			}

			--indent;
		}

		--indent;
	}

	--indent;

	if (needDel)
		delete tri;
}

void ElMaxPlugin::ExportHelperObject(INode* node)
{
	// Export node type
	outfile << indent.str() << "HelperObject" << endl;
	++indent;

	// Export node header
	ExportNodeHeader(node);

	// We don't really know what kind of helper this is, but by exporting
	// the Classname of the helper object, the importer has a chance to
	// identify it.
	Object* helperObj = node->EvalWorldState(0).obj;
	if (helperObj)
	{
		TSTR className;
		helperObj->GetClassName(className);
		outfile << indent.str() << "HelperClass " << className << endl;
	}

	// Export node transformation
	ExportNodeTM(node);

	// Export bounding box
	if (helperObj)
	{
		TimeValue	t = GetStaticFrame();
		Matrix3		oTM = node->GetObjectTM(t);
		Box3		bbox;

		helperObj->GetDeformBBox(t, bbox, &oTM);

		outfile << indent.str() << "BoundingBoxMin " << bbox.pmin.x << ' ' << bbox.pmin.y << ' ' << bbox.pmin.z << endl;
		outfile << indent.str() << "BoundingBoxMax " << bbox.pmax.x << ' ' << bbox.pmax.y << ' ' << bbox.pmax.z << endl;
	}

	--indent;
}

void ElMaxPlugin::DumpMaterial(Mtl* mtl, int mtlID, int subID)
{
	if (subID == -1)	// Top level material
		outfile << indent.str() << "Material " << mtlID << endl;
	else
		outfile << indent.str() << "SubMaterial " << subID << endl;
	++indent;

	outfile << indent.str() << "MaterialName " << mtl->GetName() << endl;

	Color amb = mtl->GetAmbient();
	Color diff = mtl->GetDiffuse();
	Color spec = mtl->GetSpecular();

	outfile << indent.str() << amb.r << " " << amb.g << " " << amb.b << endl;
	outfile << indent.str() << diff.r << " " << diff.g << " " << diff.b << endl;
	outfile << indent.str() << spec.r << " " << spec.g << " " << spec.b << endl;

	// If it is a standard material we can see if the map is enabled.
	if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
	{
		int numSubTexmaps = mtl->NumSubTexmaps();
		for (int i = 0; i < numSubTexmaps; ++i)
		{
			if (!((StdMat*)mtl)->MapEnabled(i))
				continue;

			Texmap* subTex = mtl->GetSubTexmap(i);
			if (subTex)
			{
				DumpTexture(subTex, mtl->ClassID(), i);
			}
		}
	}

	int numSubMtls = mtl->NumSubMtls();
	if (numSubMtls > 0) 
	{
		outfile << indent.str() << "NumSubMtls " << numSubMtls << endl;

		for (int i = 0; i < numSubMtls; ++i)
		{
			Mtl* subMtl = mtl->GetSubMtl(i);
			if (subMtl)
				DumpMaterial(subMtl, mtlID, i);
		}
	}

	--indent;
}

void ElMaxPlugin::DumpTexture(Texmap* tex, Class_ID cid, int subNo)
{
	if (cid == Class_ID(0,0))
	{
		// Scene EnvMap
	}
	// We just take care of the bitmap texture as a diffuse map
	else if (cid == Class_ID(DMTL_CLASS_ID, 0))
	{
		if (subNo == ID_DI)
		{
			if (tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00))
			{
				outfile << indent.str() << "Bitmap " << ((BitmapTex *)tex)->GetMapName() << endl;
			}
		}
	}
	else
	{
		// MapGeneric
	}

	++indent;
	int numSubTexmaps = tex->NumSubTexmaps();
	for (int i = 0; i < numSubTexmaps; ++i)
	{
		DumpTexture(tex->GetSubTexmap(i), tex->ClassID(), i);
	}
	--indent;
}

TriObject* ElMaxPlugin::GetTriObjectFromNode(INode *node, TimeValue t, BOOL &deleteIt)
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(t).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
	{ 
		TriObject *tri = (TriObject *)obj->ConvertToType(t, Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri)
			deleteIt = TRUE;
		return tri;
	}
	else
	{
		return NULL;
	}
}

Point3 ElMaxPlugin::GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv)
{
	Face* f = &mesh->faces[faceNo];
	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL)
	{
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup)
	{
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1)
		{
			vertexNormal = rv->rn.getNormal();
		}
		else
		{
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++)
			{
				if (rv->ern[i].getSmGroup() & smGroup)
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh->getFaceNormal(faceNo);
	}

	return vertexNormal;
}

