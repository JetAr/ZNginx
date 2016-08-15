/**********************************************************************
 *<
	FILE: ElMaxPlugin.h

	DESCRIPTION:	Includes for Plugins

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2003, All Rights Reserved.
 **********************************************************************/

#ifndef __ElMaxPlugin__H
#define __ElMaxPlugin__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "stdmat.h"
#include "utilapi.h"
#include "iskin.h"
#include "modstack.h"
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>

#define clampNan(x)		(_isnan((x)) ? 0.0f : (x))

using namespace std;

// object keeper
template<typename T> class ElKeeper
{
public:
	void Clear();
	bool Add(T* obj);
	int GetID(T* obj);
	int Count();
	T* Get(int id);

protected:
	std::vector<T*> mArray;
};

#include "ElKeeper.inl"

// indent class for ascii output
class Indent
{
public:
	Indent() : mIndent("") {}
	Indent(const string str) : mIndent(str) {}
	Indent(const Indent& obj) : mIndent(obj.mIndent) {}

	Indent& operator++()
	{
		mIndent += '\t';

		return *this;
	}

	const Indent operator++(int)
	{
		Indent old(*this);
		++(*this);
		return old;
	}

	Indent& operator--()
	{
		if (!mIndent.empty())
			mIndent.erase(0, 1);

		return *this;
	}

	const Indent operator--(int)
	{
		Indent old(*this);
		--(*this);
		return old;
	}

	const TCHAR* str() { return mIndent.c_str(); }

protected:
	std::string mIndent;
};

// ElMaxPlugin
class ElMaxPlugin : public UtilityObj
{
public:
	HWND			hPanel;
	IUtil			*iu;
	Interface		*ip;

	// Template methods
	ElMaxPlugin();
	~ElMaxPlugin();
	
	void			BeginEditParams(Interface *ip,IUtil *iu);
	void			EndEditParams(Interface *ip,IUtil *iu);

	void			Init(HWND hWnd);
	void			Destroy(HWND hWnd);
	void			DeleteThis() {}

	// Export methods
	bool			ExportMesh(LPCTSTR filename);
	bool			ExportMeshAnim(LPCTSTR filename);
	bool			ExportSkeleton(LPCTSTR filename);
	bool			ExportSkeletalAnim(LPCSTR filename);
	void			ExportGlobalInfo();

	void			DumpMatrix3(Matrix3* m);

	BOOL			TMNegParity(Matrix3 &m);

	// Mesh Export methods
	void			nodeEnum(INode* node);
	void			FindMaterials(INode* node);

	void			ExportMaterialList();
	void			ExportGeomObject(INode* node);
	void			ExportNodeHeader(INode* node);
	void			ExportNodeTM(INode* node);
	void			ExportMaterial(INode* node);
	void			ExportMesh(INode* node, TimeValue t);
	void			ExportHelperObject(INode* node);

	void			DumpMaterial(Mtl* mtl, int mtlID, int subID);
	void			DumpTexture(Texmap* tex, Class_ID cid, int subNo);

	TriObject*		GetTriObjectFromNode(INode *node, TimeValue t, BOOL &deleteIt);
	Point3			GetVertexNormal(Mesh* mesh, int faceNo, RVertex* rv);

	// MeshAnim Export methods
	void			nodeEnumForAnim(INode* node);
	void			ExportGeomObjectAnim(INode* node);
	void			ExportMeshFrame(INode* node, TimeValue t, TimeValue base);

	// Skeleton Export methods
	void			PreProcess(INode* node);
	void			ExportBoneList();

	void			DumpBone(INode* bone);

	void			FindBones(INode* node);
	ISkin*			FindSkinModifier(INode* node);

	// SkeletalAnim Export methods
	void			ExportSkeletalAnim();
	
	void			DumpBoneAnim(INode* bone, int boneID, TimeValue t);

	void			getBoneLocalTM(INode* bone, TimeValue t, Matrix3& m);

	// Interface to member variables
	inline TimeValue GetStaticFrame() { return nStaticFrame; }
	inline int		GetMeshAnimFrameStart() { return nMeshAnimStartFrame; }
	inline int		GetMeshAnimFrameEnd() { return nMeshAnimEndFrame; }
	inline int		GetMeshAnimFrameStep() { return nMeshAnimKeyFrameStep; }
	inline int		GetSkeletalAnimFrameStart() { return nSkeletalAnimStartFrame; }
	inline int		GetSkeletalAnimFrameEnd() { return nSkeletalAnimEndFrame; }
	inline int		GetSkeletalAnimFrameStep() { return nSkeletalAnimKeyFrameStep; }
	inline Interface* GetInterface() { return ip; }

	inline void		SetStaticFrame(TimeValue val) { nStaticFrame = val; }
	inline void		SetMeshAnimFrameStart(int val) { nMeshAnimStartFrame = val; }
	inline void		SetMeshAnimFrameEnd(int val) { nMeshAnimEndFrame = val; }
	inline void		SetMeshAnimFrameStep(int val) { nMeshAnimKeyFrameStep = val; }
	inline void		SetSkeletalAnimFrameStart(int val) { nSkeletalAnimStartFrame = val; }
	inline void		SetSkeletalAnimFrameEnd(int val) { nSkeletalAnimEndFrame = val; }
	inline void		SetSkeletalAnimFrameStep(int val) { nSkeletalAnimKeyFrameStep = val; }

protected:
	TimeValue		nStaticFrame;
	int				nMeshAnimStartFrame;
	int				nMeshAnimEndFrame;
	int				nMeshAnimKeyFrameStep;
	int				nSkeletalAnimStartFrame;
	int				nSkeletalAnimEndFrame;
	int				nSkeletalAnimKeyFrameStep;

	ElKeeper<Mtl>	mtlList;
	ElKeeper<INode>	boneList;

	ofstream		outfile;
	Indent			indent;
};

extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;

#endif
