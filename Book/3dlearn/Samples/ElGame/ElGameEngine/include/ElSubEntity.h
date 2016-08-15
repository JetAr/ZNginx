#ifndef __ElSubEntity_H__
#define __ElSubEntity_H__

#include "ElDefines.h"
#include "ElSubMesh.h"
#include "ElArray.h"
#include "ElSkeleton.h"
#include "ElAnimation.h"
#include "ElMeshAnimation.h"
#include <vector>

#define ElMaxVertexWeight					4
#define ElBlendMatrixIdx(w, x, y, z)		D3DCOLOR_ARGB(w, x, y, z)

class ElSubEntity
{
public:
	struct VERTEX
	{
	public:
		D3DXVECTOR3			pos;
		D3DXVECTOR3			normal;
		D3DXVECTOR2			tex;

		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	};

	struct BLENDVERTEX
	{
	public:
		D3DXVECTOR3			pos;
		float				weights[ElMaxVertexWeight - 1];
		DWORD				matrixIndices;
		D3DXVECTOR3			normal;
		D3DXVECTOR2			tex;

		static const DWORD FVF = D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1;
	};

public:
	ElSubEntity(ElSubMesh* subMesh);
	virtual ~ElSubEntity();

	bool		initialise();
	void		updateVertexBuffer(bool forceCreate = false);

	void		update(const ElAnimationState& as);
	void		render(float alpha);

	ElSubMesh*	getSubMesh();
	void		setAnimation(ElSubMeshAnimation* sma);

	void		_notifySkeletonAttached(ElSkeleton* skeleton);

protected:
	void		doMeshAnimation(const ElAnimationState& as);
	void		doSkeletalAnimation();

protected:
	ElSubMesh*					mSubMesh;

	IDirect3DVertexBuffer9*		mpVertexBuffer;
	
	// Due to the differences between normal VERTEX and blend VERTEX, we need to use
	// two sets of variables to record the vertices. Doing this likely won't affect
	// the performance of simple sample but it should be done in complex engine.
	ElArray<VERTEX>				mVertices;
	ElArray<BLENDVERTEX>		mBlendVertices;
	void*						mBuffPtr;
	DWORD						mFVF;
	unsigned int				mVertexSize;
	int							mNumVertices;

	ElSubMeshAnimation*			mAnimation;
};

typedef std::vector<ElSubEntity*> ElSubEntityList;
typedef std::vector<ElSubEntity*>::iterator ElSubEntityIterator;

#endif //__ElSubEntity_H__