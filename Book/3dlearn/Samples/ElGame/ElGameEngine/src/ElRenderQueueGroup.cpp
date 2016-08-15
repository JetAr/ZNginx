#include "ElApplicationPCH.h"
#include "ElRenderQueueGroup.h"
#include <algorithm>

ElRenderQueueGroup::ElRenderQueueGroup(ElRenderQueue* parent)
{

}

ElRenderQueueGroup::~ElRenderQueueGroup()
{

}

void ElRenderQueueGroup::addRenderable(ElRenderable* renderable)
{
	mRenderableList.push_back(renderable);
}

void ElRenderQueueGroup::sort(ElCamera* cam)
{
	std::stable_sort(mRenderableList.begin(), mRenderableList.end(), DepthSortDescendingLess(cam));
}

void ElRenderQueueGroup::clear()
{
	mRenderableList.clear();
}

void ElRenderQueueGroup::renderObjectsList(ElCamera* camera, TimeValue t)
{
	RenderableList::iterator i;
	RenderableList::iterator iend = mRenderableList.end();
	for (i = mRenderableList.begin(); i != iend; ++i)
	{
		(*i)->render(camera, t);
	}
}