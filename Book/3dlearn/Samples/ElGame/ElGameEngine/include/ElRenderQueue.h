#ifndef __ElRenderQueue_H__
#define __ElRenderQueue_H__

#include "ElDefines.h"
#include "ElRenderQueueGroup.h"
#include <map>

/*
	Enumeration of queue groups, by which the application may group queued renderables
	so that they are rendered together with events in between
*/
enum ElRenderQueueGroupID
{
	RENDER_QUEUE_FIRST = 0,
	//// Use this queue for objects which must be rendered first e.g. backgrounds
	//RENDER_QUEUE_BACKGROUND = RENDER_QUEUE_FIRST,
	// First queue (after backgrounds), used for skyboxes if rendered first
	RENDER_QUEUE_SKIES_EARLY = RENDER_QUEUE_FIRST,
	RENDER_QUEUE_WORLD_GEOMETRY_1,
	RENDER_QUEUE_WORLD_GEOMETRY_2,
	// The default render queue
	RENDER_QUEUE_MAIN,
	RENDER_QUEUE_TRANSPARENT_1,
	//RENDER_QUEUE_TRANSPARENT_2,
	//// Penultimate queue(before overlays), used for skyboxes if rendered last
	//RENDER_QUEUE_SKIES_LATE,
	//// Use this queue for objects which must be rendered last e.g. overlays
	//RENDER_QUEUE_OVERLAY,
	//// Final possible render queue, don't exceed this
	RENDER_QUEUE_MAX
};

class ElCamera;
class ElMovableObject;

class ElRenderQueue
{
public:
	typedef std::map<unsigned char, ElRenderQueueGroup*> RenderQueueGroupMap;
	typedef RenderQueueGroupMap::iterator QueueGroupIterator;
	typedef RenderQueueGroupMap::const_iterator ConstQueueGroupIterator;

public:
	ElRenderQueue();
	virtual ~ElRenderQueue();

	void clear();
	ElRenderQueueGroup* getQueueGroup(unsigned char groupID);
	void addRenderable(ElRenderable* renderable, unsigned char groupId);

	void processVisibleObject(ElMovableObject* mo, ElCamera* cam);
	void renderQueueGroupObjects(ElCamera* camera, TimeValue t);

protected:
	RenderQueueGroupMap	mGroups;
};

#endif //__ElRenderQueue_H__