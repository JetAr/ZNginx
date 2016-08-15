#include "ElApplicationPCH.h"
#include "ElSkeletonSerializer.h"
#include "ElDefines.h"

ElSkeletonSerializer::ElSkeletonSerializer()
{

}

ElSkeletonSerializer::~ElSkeletonSerializer()
{
}

bool ElSkeletonSerializer::loadSkeleton(LPCSTR filename, ElSkeleton* skeleton)
{
	// open stream
	infile.open(filename);
	if (!infile.good())
		return false;

	// import the entity
	infile >> token;
	while (!infile.eof())
	{
		if (token.compare("ElMaxPlugin") == 0)
		{
			importGlobalInfo();
		}
		else if (token.compare("BoneList") == 0)
		{
			importBoneList(skeleton);
		}
		else
		{
			// something is wrong
			break;
		}
	}

	// optimize the skeleton, a tree of bones will be built finally
	skeleton->optimize();

	// close stream
	infile.close();
	infile.clear();

	return true;
}

bool ElSkeletonSerializer::loadSkeletalAnimation(LPCSTR filename, ElSkeletalAnimation* skeletalAnim)
{
	// open stream
	infile.open(filename);
	if (!infile.good())
		return false;

	// import the entity
	infile >> token;
	while (!infile.eof())
	{
		if (token.compare("ElMaxPlugin") == 0)
		{
			importGlobalInfo();
		}
		else if (token.compare("SkeletalAnimList") == 0)
		{
			importSkeletalAnim(skeletalAnim);
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

void ElSkeletonSerializer::importGlobalInfo()
{
	infile >> token;
	if (token.compare("Version") == 0)
	{
		int version;
		infile >> version;
		infile >> token;
	}
}

void ElSkeletonSerializer::importBoneList(ElSkeleton* skeleton)
{
	int numBones;
	infile >> numBones;

	// import each bone in the list
	infile >> token;
	while (!infile.eof() && token.compare("Bone") == 0)
	{
		ElBone* bone = ElNew ElBone;

		char name[EL_MAX_PATH] = {0};
		infile.get();
		infile.getline(name, sizeof(name));
		bone->setName(name);

		infile >> token;
		if (token.compare("InitTM") == 0)
		{
			importMatrix(bone->mInitTM);
			D3DXMatrixInverse(&bone->mInverseInitTM, NULL, &bone->mInitTM);

			infile >> token;
		}

		if (token.compare("Parent") == 0)
		{
			char parent[EL_MAX_PATH] = {0};
			infile.get();
			infile.getline(parent, sizeof(parent));
			bone->mParentName = parent;

			infile >> token;
		}

		skeleton->addBone(bone);
	}
}

void ElSkeletonSerializer::importSkeletalAnim(ElSkeletalAnimation* skeletalAnim)
{
	int numBones = 0;

	infile >> token;
	if (token.compare("NumBones") == 0)
	{
		infile >> numBones;
		infile >> token;
	}

	// import bone matrix according to frame
	while (!infile.eof() && token.compare("Frame") == 0)
	{
		ElSkeletalAnimation::Frame* frame = ElNew ElSkeletalAnimation::Frame;
		frame->dm.assign(numBones);
		frame->tm.assign(numBones);
		frame->boneId.assign(numBones);

		int frameId;
		infile >> frameId >> frame->t;

		for (int b = 0; b < numBones; ++b)
		{
			infile >> token >> frame->boneId[b];
			importMatrix(frame->tm[b]);
			D3DXMatrixDecompose(&frame->dm[b].scale, &frame->dm[b].rotation, &frame->dm[b].translation, &frame->tm[b]);
			assert(token.compare("BoneID") == 0);
		}

		skeletalAnim->addFrame(frame);

		infile >> token;
	}
}

void ElSkeletonSerializer::importMatrix(D3DXMATRIX& m)
{
	D3DXMatrixIdentity(&m);

	infile >> m._11 >> m._13 >> m._12
		>> m._31 >> m._33 >> m._32
		>> m._21 >> m._23 >> m._22
		>> m._41 >> m._43 >> m._42;
}

