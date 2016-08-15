#ifndef __ElWireBoundingBox_H__
#define __ElWireBoundingBox_H__

#include "ElD3DHeaders.h"
#include "ElAxisAlignedBox.h"
#include "ElArray.h"
#include "ElDefines.h"
#include "ElRenderable.h"

class ElWireBoundingBox : public ElRenderable
{
public:
	struct VERTEX
	{
		float	x, y, z;
		DWORD	color;
		
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	};

public:
	ElWireBoundingBox();
	virtual ~ElWireBoundingBox();

	void setBoundingBoxColor(ColorValue color);
	void setupBoundingBoxVertices(const ElAxisAlignedBox& aab);
	virtual void render(ElCamera* camera, TimeValue t);

protected:
	ElArray<VERTEX>		mVertices;
	ColorValue			mColor;
};

#endif //__ElWireBoundingBox_H__