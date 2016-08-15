#ifndef __ElRenderable_H__
#define __ElRenderable_H__

#include "ElCamera.h"

class ElRenderable
{
public:
	ElRenderable();
	virtual ~ElRenderable();

	virtual void render(ElCamera* camera, TimeValue t) = 0;
	// we only need this for transparent objects
	virtual float getSquaredViewDepth(ElCamera* cam) const { return 0.0f; }
};

#endif //__ElRenderable_H__