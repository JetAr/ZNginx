#ifndef __ElMeshAnimation_H__
#define __ElMeshAnimation_H__

#include "ElDefines.h"
#include "ElArray.h"
#include "ElSubMesh.h"
#include <vector>
#include <string>

class ElSubMeshAnimation
{
public:
	struct Frame
	{
		TimeValue						t;
		ElArray<ElSubMesh::VertexData>	verts;
		ElArray<ElSubMesh::NormalData>	normals;
	};
	typedef std::vector<Frame*> MeshFrameList;

	ElSubMeshAnimation();
	virtual ~ElSubMeshAnimation();

	void			addFrame(Frame* frame);
	Frame*			getFrame(int idx);
	int				getNumFrames();

protected:
	MeshFrameList	mFrameList;
};

/**
	Set of sub mesh animations, they may have different frame counts, but
	must share the same key frame time
*/
class ElMeshAnimation
{
public:
	typedef std::map<std::string, ElSubMeshAnimation*> SubMeshAnimSet;
	typedef SubMeshAnimSet::iterator SubMeshAnimIterator;
	typedef SubMeshAnimSet::const_iterator SubMeshAnimConstIterator;
	typedef SubMeshAnimSet::value_type SubMeshAnimValue;

public:
	ElMeshAnimation();
	virtual ~ElMeshAnimation();

	bool				load(LPCTSTR filename);

	bool				addAnimation(const std::string& subMeshName, ElSubMeshAnimation* animation);
	ElSubMeshAnimation* getAnimation(const std::string& subMeshName);
	ElSubMeshAnimation* getLongestAnimation();

protected:
	SubMeshAnimSet		mAnimationSet;
};

#endif //__ElMeshAnimation_H__