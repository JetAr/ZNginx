#include "ElMaxPlugin.h"

void ElMaxPlugin::nodeEnumForAnim(INode* node)
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
			ExportGeomObjectAnim(node);
			break;
		case CAMERA_CLASS_ID:
			break;
		case LIGHT_CLASS_ID:
			break;
		case SHAPE_CLASS_ID:
			break;
		case HELPER_CLASS_ID:
			break;
		}
	}

	int count = node->NumberOfChildren();
	for (int i = 0; i < count; ++i)
	{
		nodeEnumForAnim(node->GetChildNode(i));
	}
}

void ElMaxPlugin::ExportGeomObjectAnim(INode* node)
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
	
	// Get animation range
	int ticksPerFrame = GetTicksPerFrame();
	Interval animRange = ip->GetAnimRange();
	animRange.SetStart(max(GetMeshAnimFrameStart() * ticksPerFrame, animRange.Start()));
	animRange.SetEnd(min(GetMeshAnimFrameEnd() * ticksPerFrame, animRange.End()));

	// Get validity of the object
	Interval objRange = os.obj->ObjectValidity(animRange.Start());

	// If the animation range is not fully included in the validity
	// interval of the object, then we're animated.
	if (!objRange.InInterval(animRange))
	{
		// Export node type
		outfile << indent.str() << "GeomObject" << endl;
		++indent;

		// Export node name
		outfile << indent.str() << "NodeName " << node->GetName() << endl;

		// Export animation
		outfile << indent.str() << "MeshAnimation" << endl;
		++indent;

		TimeValue t = animRange.Start();

		while (true)
		{
			// This may seem strange, but the object in the pipeline
			// might not be valid anymore.
			os = node->EvalWorldState(t);
			objRange = os.obj->ObjectValidity(t);
			t = objRange.Start() < animRange.Start() ? animRange.Start() : objRange.Start();

			// Export the mesh definition at this frame
			// use ExportMeshFrame or ExportMesh, think about it.
			ExportMeshFrame(node, t, animRange.Start());

			if (objRange.End() >= animRange.End())
				break;
			else
				t = (objRange.End() / GetTicksPerFrame() + GetMeshAnimFrameStep()) * GetTicksPerFrame();
		}

		--indent;
		--indent;
	}
}

void ElMaxPlugin::ExportMeshFrame(INode* node, TimeValue t, TimeValue base)
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

	// Export frame info
	outfile << indent.str() << "Frame" << endl;
	++indent;
	outfile << indent.str() << "TimeValue " << t - base << endl;

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

	// Export mesh (face + vertex) normals
	int numFaces = mesh->getNumFaces();
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
	--indent;

	if (needDel)
		delete tri;
}