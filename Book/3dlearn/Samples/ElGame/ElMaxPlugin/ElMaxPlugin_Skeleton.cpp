#include "ElMaxPlugin.h"

void ElMaxPlugin::PreProcess(INode* node)
{
	// Add the bones to out material list
	// Null entries are ignored when added...
	FindBones(node);

	// For each child of this node, we recurse into ourselves 
	// and increment the counter until no more children are found.
	for (int i = 0; i < node->NumberOfChildren(); ++i)
	{
		PreProcess(node->GetChildNode(i));
	}
}

void ElMaxPlugin::ExportBoneList()
{
	int numBones = boneList.Count();
	outfile << indent.str() << "BoneList " << numBones << endl;

	++indent;
	for (int i = 0; i < numBones; ++i)
	{
		DumpBone(boneList.Get(i));
	}
	--indent;
}

void ElMaxPlugin::DumpBone(INode* bone)
{
	outfile << indent.str() << "Bone " << bone->GetName() << endl;
	++indent;

	outfile << indent.str() << "InitTM" << endl;
	++indent;
	Matrix3 m;
	m = bone->GetNodeTM(GetStaticFrame());
	DumpMatrix3(&m);
	--indent;

	INode* parent = bone->GetParentNode();
	if (parent && boneList.GetID(parent) != -1)
		outfile << indent.str() << "Parent " << parent->GetName() << endl;

	--indent;
}

void ElMaxPlugin::FindBones(INode* node)
{
	ISkin* skin = FindSkinModifier(node);
	if (skin)
	{
		int numBones = skin->GetNumBones();
		for (int b = 0; b < numBones; ++b)
		{
			INode* bone = skin->GetBone(b);
			boneList.Add(bone);
		}
	}
}

ISkin* ElMaxPlugin::FindSkinModifier(INode* node)
{
	Object* objRef = node->GetObjectRef();
	if (objRef == 0)
		return NULL;

	while (objRef->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		IDerivedObject* derivedObj = static_cast<IDerivedObject*>(objRef);
		int numModifiers = derivedObj->NumModifiers();
		for (int i = 0; i < numModifiers; ++i)
		{
			Modifier* modifier = derivedObj->GetModifier(i);
			if (modifier->ClassID() == SKIN_CLASSID)
				return (ISkin*)modifier->GetInterface(I_SKIN);
		}

		objRef = derivedObj->GetObjRef();
	}

	return NULL;
}