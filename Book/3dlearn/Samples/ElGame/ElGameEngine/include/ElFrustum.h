#ifndef __ElFrustum_H__
#define __ElFrustum_H__

#include "ElD3DHeaders.h"
#include "ElMovableObject.h"
#include "ElAxisAlignedBox.h"

class ElSceneNode;

class ElFrustum : public ElMovableObject
{
public:
	enum ProjectionType
	{
		PT_ORTHOGRAPHIC,
		PT_PERSPECTIVE
	};

	enum FrustumPlane
	{
		FRUSTUM_PLANE_NEAR = 0,
		FRUSTUM_PLANE_FAR,
		FRUSTUM_PLANE_LEFT,
		FRUSTUM_PLANE_RIGHT,
		FRUSTUM_PLANE_TOP,
		FRUSTUM_PLANE_BOTTOM
	};

public:
	ElFrustum();
	virtual ~ElFrustum();

	virtual void setProjectionParameters(ProjectionType type, float param1,/* width / FOV */ float param2,/* height / aspect */ float nearDist, float farDist);

	virtual void enableReflection(const D3DXPLANE& plane);
	virtual void disableReflection();
	virtual bool isReflected() const;
	virtual const D3DXMATRIX& getReflectionMatrix() const;
	virtual const D3DXPLANE& getReflectionPlane() const;

	virtual const D3DXMATRIX& getProjectionMatrix();
	virtual const D3DXMATRIX& getViewMatrix();

	// Tests whether the given vertex is visible in the Frustum.
	virtual bool isVisible(const D3DXVECTOR3& vert, FrustumPlane* culledBy = 0);
	// Tests whether the given container is visible in the Frustum.
	virtual bool isVisible(const D3DXVECTOR3& centre, const D3DXVECTOR3& halfSize, FrustumPlane* culledBy = 0);
	virtual bool isVisible(const ElAxisAlignedBox& bound, FrustumPlane* culledBy = 0);

	virtual void _updateRenderQueue(ElRenderQueue* queue, ElCamera* cam) {}

protected:
	virtual bool isFrustumOutOfDate();
	virtual bool isViewOutOfDate();
	
	virtual void invalidateFrustum();
	virtual void invalidateView();

	virtual void updateFrustum();
	virtual void updateFrustumImp();
	virtual void updateView();
	virtual void updateViewImp();
	virtual void updateFrustumPlanes();
	virtual void updateFrustumPlanesImp();

	virtual const D3DXVECTOR3& getPositionForViewUpdate() const;
	virtual const D3DXQUATERNION& getOrientationForViewUpdate() const;

protected:
	// Orthographic or perspective?
	ProjectionType	mProjType;

	float			mFarDist;
	float			mNearDest;
	// just used for orthographic
	float			mWidth;
	float			mHeight;
	// just used for perspective
	float			mFOV;
	float			mAspect;

	D3DXPLANE		mFrustumPlanes[6];

	D3DXQUATERNION	mLastParentOrientation;
	D3DXVECTOR3		mLastParentPosition;

	D3DXMATRIX		mProjMatrix;
	D3DXMATRIX		mViewMatrix;

	bool			mReflect;
	D3DXPLANE		mReflectPlane;
	D3DXMATRIX		mReflectMatrix;

	bool			mRecalcFrustum;
	bool			mRecalcView;
	bool			mRecalcFrustumPlanes;
};

#endif //__ElFrustum_H__