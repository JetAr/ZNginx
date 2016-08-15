#ifndef __ElSceneNode_H__
#define __ElSceneNode_H__

#include "ElDefines.h"
#include "ElD3DHeaders.h"
#include "ElNode.h"
#include "ElMovableObject.h"
#include "ElWireBoundingBox.h"
#include "ElAxisAlignedBox.h"
#include "ElRaySceneQuery.h"
#include "ElRenderQueue.h"
#include <map>
#include <string>

class ElCamera;

class ElSceneNode : public ElNode
{
public:
	class Listener
	{
	public:
		Listener() {}
		virtual ~Listener() {}
		virtual void nodeUpdated(const ElSceneNode*) {}
		virtual void nodeDestroyed(const ElSceneNode*) {}
		virtual void nodeAttached(const ElSceneNode*) {}
		virtual void nodeDetached(const ElSceneNode*) {}
	};

	typedef std::map<std::string, ElMovableObject*> ObjectMap;
	typedef ObjectMap::iterator ObjectIterator;
	typedef ObjectMap::const_iterator ConstObjectIterator;
	typedef ObjectMap::value_type ObjectValue;

public:
	ElSceneNode();
	ElSceneNode(const std::string& name);
	virtual ~ElSceneNode();

	virtual void setParent(ElSceneNode* parent);

	virtual const D3DXQUATERNION& getOrientation();
	virtual const D3DXVECTOR3& getScale();
	virtual const D3DXVECTOR3& getPosition();

	virtual void setOrientation(const D3DXQUATERNION& q);
	virtual void setScale(const D3DXVECTOR3& scale);
	virtual void setPosition(const D3DXVECTOR3& pos);

	virtual void scale(float x, float y, float z);
	
	virtual void pitch(float angle);
	virtual void roll(float angle);
	virtual void yaw(float angle);
	virtual void rotate(const D3DXVECTOR3& axis, const float angle);

	virtual const D3DXQUATERNION& getDerivedOrientation() const;
	virtual const D3DXVECTOR3& getDerivedScale() const;
	virtual const D3DXVECTOR3& getDerivedPosition() const;

	virtual const D3DXMATRIX& getCachedTransform() const;

	virtual bool getInheritOrientation() const;
	virtual bool getInheritScale() const;

	virtual void setInheritOrientation(bool inherit);
	virtual void setInheritScale(bool inherit);

	virtual void needUpdate();
	
	virtual void update();
	virtual void _findVisibleObjects(ElCamera* cam, ElRenderQueue* queue, bool includeChildren = true);

	virtual void attachObject(ElMovableObject* obj);
	virtual void dettachObject(ElMovableObject* obj);

	virtual ElSceneNode* createChildSceneNode(const std::string& name);

	virtual const ElAxisAlignedBox& getWorldAABB() const;
	virtual void showBoundingBox(bool show);
	virtual void setBoundingBoxColor(ColorValue color);

	virtual void setVisible(bool visible, bool cascade = true);

	virtual void intersects(ElRaySceneQuery& raySceneQuery);

	virtual void setListener(Listener* listener);
	virtual Listener* getListener() const;

protected:
	virtual void _updateFromParent();
	virtual void updateFromParentImpl();
	virtual void _makeCachedTransform();
	
	virtual void _addBoundingBoxToQueue(ElRenderQueue* queue);
	virtual void _updateBounds();

protected:
	D3DXQUATERNION mOrientation;
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mScale;

	D3DXQUATERNION mDerivedOrientation;
	D3DXVECTOR3 mDerivedPosition;
	D3DXVECTOR3 mDerivedScale;

	D3DXMATRIX mCachedTransform;

	bool mInheritOrientation;
	bool mInheritScale;

	bool mNeedUpdate;

	ObjectMap mObjectsByName;

	ElAxisAlignedBox mWorldAABB;
	bool mShowBoundingBox;
	ColorValue mBoundingBoxColor;
	ElWireBoundingBox* mWireBoundingBox;

	// Node listener - only one allowed (no list) for size & performance reasons.
	Listener* mListener;
};

#endif //__ElSceneNode_H__