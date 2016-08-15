#ifndef __ElMovableObject_H__
#define __ElMovableObject_H__

#include "ElD3DHeaders.h"
#include "ElDefines.h"
#include "ElAxisAlignedBox.h"
#include <string>

class ElSceneNode;
class ElCamera;
class ElRaySceneQuery;
class ElRenderQueue;

class ElMovableObject
{
public:
	ElMovableObject();
	virtual ~ElMovableObject();

	virtual void	update();
	virtual void	render(ElCamera* camera, TimeValue t);

	virtual void	setName(const std::string& name);
	virtual const std::string& getName();
	virtual void	setAttached(ElSceneNode* parent);
	virtual bool	isAttached();

	virtual const	ElAxisAlignedBox& getWorldBoundingBox(bool derive = false);
	virtual const	ElAxisAlignedBox& getBoundingBox();

	virtual ElSceneNode* getParentNode();
	virtual void	getParentNodeTransform(D3DXMATRIX& mat);

	virtual void	setVisible(bool visible);
	virtual bool	isVisible() const;

	virtual void	setRenderQueueGroup(unsigned int queueID);
	virtual unsigned int getRenderQueueGroup() const;

	virtual void	_updateRenderQueue(ElRenderQueue* queue, ElCamera* cam) = 0;

	virtual void	intersects(ElRaySceneQuery& raySceneQuery);

protected:
	ElSceneNode*		mParentNode;
	std::string			mName;

	ElAxisAlignedBox	mWorldAABB;
	bool				mVisible;

	unsigned int		mRenderQueueID;
};

#endif //__ElMovableObject_H__