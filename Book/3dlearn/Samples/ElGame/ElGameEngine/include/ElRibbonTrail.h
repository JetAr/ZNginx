#ifndef __ElRibbonTrail_H__
#define __ElRibbonTrail_H__

#include "ElBillboardChain.h"
#include "ElD3DHeaders.h"
#include "ElSceneNode.h"
#include <vector>

class ElRibbonTrail : public ElBillboardChain, public ElSceneNode::Listener
{
public:
	ElRibbonTrail();
	virtual ~ElRibbonTrail();

	virtual void setTrailLength(float len);
	virtual void setTrailWidth(float width);

	virtual void setMaxChainElements(size_t maxElements);
	virtual void updateTrail(const D3DXVECTOR3& position);
	virtual void resetTrail();

	virtual void update();

	virtual float getSquaredViewDepth(ElCamera* cam) const;

	void attachNode(ElSceneNode* node);
	virtual void nodeUpdated(const ElSceneNode* node);
	virtual void nodeDestroyed(const ElSceneNode* node);

protected:
	void _createChainElements();
	void _updateChainElements();

protected:
	float		mTrailWidth;
	float		mTrailLength;
	float		mElemLength;
	float		mSquaredElemLength;

	typedef std::vector<D3DXVECTOR3> TraceList;
	TraceList	mTraceList;
	bool		mTraceDirty;
};

#endif //__ElRibbonTrail_H__