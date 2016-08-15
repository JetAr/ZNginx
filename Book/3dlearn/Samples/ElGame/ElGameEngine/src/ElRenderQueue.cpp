#include "ElApplicationPCH.h"
#include "ElRenderQueue.h"
#include "ElMovableObject.h"
#include "ElCamera.h"

ElRenderQueue::ElRenderQueue()
{

}

ElRenderQueue::~ElRenderQueue()
{
	// Destroy the queues for good
	RenderQueueGroupMap::iterator i, iend;
	i = mGroups.begin();
	iend = mGroups.end();
	for (; i != iend; ++i)
	{
		ElSafeDelete(i->second);
	}
	mGroups.clear();
}

void ElRenderQueue::clear()
{
	// Clear the queues
	RenderQueueGroupMap::iterator i, iend;
	i = mGroups.begin();
	iend = mGroups.end();
	for (; i != iend; ++i)
	{
		i->second->clear();
	}
}

ElRenderQueueGroup* ElRenderQueue::getQueueGroup(unsigned char groupID)
{
	// Find group
	ElRenderQueueGroup* group = 0;
	RenderQueueGroupMap::iterator groupIt;

	groupIt = mGroups.find(groupID);
	if (groupIt == mGroups.end())
	{
		// Insert new
		group = ElNew ElRenderQueueGroup(this);
		mGroups.insert(RenderQueueGroupMap::value_type(groupID, group));
	}
	else
	{
		group = groupIt->second;
	}

	return group;
}

void ElRenderQueue::addRenderable(ElRenderable* renderable, unsigned char groupId)
{
	// Find group
	ElRenderQueueGroup* group = getQueueGroup(groupId);

	group->addRenderable(renderable);
}

void ElRenderQueue::processVisibleObject(ElMovableObject* mo, ElCamera* cam)
{
	if (mo->isVisible())
		mo->_updateRenderQueue(this, cam);
}

void ElRenderQueue::renderQueueGroupObjects(ElCamera* camera, TimeValue t)
{
	QueueGroupIterator i, iend;
	iend = mGroups.end();
	for (i = mGroups.begin(); i != iend; ++i)
	{
		i->second->renderObjectsList(camera, t);
	}
}