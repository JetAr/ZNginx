#include "ElMaxPlugin.h"

void ElMaxPlugin::ExportSkeletalAnim()
{
	// calculate the interval
	int ticksPerFrame = GetTicksPerFrame();
	Interval animRange = ip->GetAnimRange();
	int frameStart = max(animRange.Start() / ticksPerFrame, GetSkeletalAnimFrameStart());
	int frameEnd = min(animRange.End() / ticksPerFrame, GetSkeletalAnimFrameEnd());
	int frameStep = GetSkeletalAnimFrameStep();

	// output
	outfile << indent.str() << "SkeletalAnimList" << endl;
	++indent;

	int numBones = boneList.Count();
	outfile << indent.str() << "NumBones " << numBones << endl;

	TimeValue tstep = frameStep * ticksPerFrame;
	TimeValue tstart = frameStart * ticksPerFrame;
	TimeValue t = tstart;
	for (int i = frameStart; i <= frameEnd; i += frameStep)
	{
		outfile << indent.str() << "Frame " << i - frameStart << ' ' << t - tstart << endl;
		++indent;

		for (int b = 0; b < numBones; ++b)
		{
			INode* bone = boneList.Get(b);
			DumpBoneAnim(bone, b, t);
		}

		--indent;
		t += tstep;
	}

	--indent;
}

void ElMaxPlugin::DumpBoneAnim(INode* bone, int boneID, TimeValue t)
{
	outfile << indent.str() << "BoneID " << boneID << endl;
	++indent;
	Matrix3 initTM, localTM;
	initTM = bone->GetNodeTM(GetStaticFrame());
	getBoneLocalTM(bone, t, localTM);
	Matrix3 m = localTM;
	DumpMatrix3(&m);
	--indent;
}

void ElMaxPlugin::getBoneLocalTM(INode* bone, TimeValue t, Matrix3& m)
{
	INode* parent = bone->GetParentNode();
	if (parent && boneList.GetID(parent) != -1)
	{
		Matrix3 parentTM = parent->GetNodeTM(t);
		m = bone->GetNodeTM(t) * Inverse(parentTM);
	}
	else
		m = bone->GetNodeTM(t);
}