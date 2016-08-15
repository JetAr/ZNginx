#ifndef __ElRenderQueueGroup_H__
#define __ElRenderQueueGroup_H__

#include "ElRenderable.h"
#include "ElCamera.h"
#include <vector>

class ElRenderQueue;

class ElRenderQueueGroup
{
public:
	typedef std::vector<ElRenderable*> RenderableList;

public:
	ElRenderQueueGroup(ElRenderQueue* parent);
	virtual ~ElRenderQueueGroup();

	void addRenderable(ElRenderable* renderable);
	void sort(ElCamera* cam);
	void clear();
	void renderObjectsList(ElCamera* camera, TimeValue t);

protected:
	struct DepthSortDescendingLess
	{
		ElCamera*	camera;

		DepthSortDescendingLess(ElCamera* cam)
			: camera(cam)
		{
		}

		bool operator()(const ElRenderable* a, const ElRenderable* b) const
		{
			// Sort by depth
			float adepth = a->getSquaredViewDepth(camera);
			float bdepth = b->getSquaredViewDepth(camera);

			// Sort DESCENDING by depth (i.e. far objects first)
			return adepth > bdepth;
		}
	};

protected:
	RenderableList mRenderableList;
};

#endif //__ElRenderQueueGroup_H__