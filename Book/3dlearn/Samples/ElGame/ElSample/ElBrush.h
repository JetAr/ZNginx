#ifndef __ElBrush_H__
#define __ElBrush_H__

#include "ElD3DHeaders.h"
#include "ElArray.h"

class ElTerrain;

class ElBrush
{
public:
	struct VERTEX
	{
		float		x, y, z;
		DWORD		color;

		static		const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	};
public:
	ElBrush();
	virtual ~ElBrush();

	bool create(ElTerrain* terrain, unsigned int numVertex, float innerRadius, float outerRadius);
	void render(D3DXVECTOR3 center);

protected:
	unsigned int			mNumVertex;
	float					mInnerRadius;
	float					mOuterRadius;
	ElArray<VERTEX>			mVertices;

	ElTerrain*				mTerrain;
};

#endif //__ElBrush_H__