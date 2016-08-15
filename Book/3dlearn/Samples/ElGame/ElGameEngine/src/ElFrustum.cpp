#include "ElApplicationPCH.h"
#include "ElFrustum.h"
#include "ElSceneNode.h"

ElFrustum::ElFrustum()
: mProjType(PT_PERSPECTIVE)
, mNearDest(0.1f)
, mFarDist(500.0f)
, mWidth(0.0f)
, mHeight(0.0f)
, mFOV(D3DX_PI / 4)
, mAspect(1.33333333333333f)
, mReflect(false)
, mRecalcFrustum(true)
, mRecalcView(true)
, mRecalcFrustumPlanes(true)
{
	D3DXQuaternionIdentity(&mLastParentOrientation);
	ZeroMemory(&mLastParentPosition, sizeof(mLastParentPosition));

	updateFrustum();
	updateView();
}

ElFrustum::~ElFrustum()
{

}

void ElFrustum::setProjectionParameters(ProjectionType type, float param1, float param2, float nearDist, float farDist)
{
	if (type == PT_PERSPECTIVE)
	{
		mFOV = param1;
		mAspect = param2;
	}
	else
	{
		mWidth = param1;
		mHeight = param2;
	}
	mProjType = type;
	mNearDest = nearDist;
	mFarDist = farDist;
	invalidateFrustum();
}

void ElFrustum::enableReflection(const D3DXPLANE& plane)
{
	mReflect = true;
	mReflectPlane = plane;
	D3DXMatrixReflect(&mReflectMatrix, &mReflectPlane);
	invalidateView();
}

void ElFrustum::disableReflection()
{
	mReflect = false;
	invalidateView();
}

bool ElFrustum::isReflected() const
{
	return mReflect;
}

const D3DXMATRIX& ElFrustum::getReflectionMatrix() const
{
	return mReflectMatrix;
}

const D3DXPLANE& ElFrustum::getReflectionPlane() const
{
	return mReflectPlane;
}

const D3DXMATRIX& ElFrustum::getProjectionMatrix()
{
	updateFrustum();
	return mProjMatrix;
}

const D3DXMATRIX& ElFrustum::getViewMatrix()
{
	updateView();
	return mViewMatrix;
}

bool ElFrustum::isVisible(const D3DXVECTOR3& vert, FrustumPlane* culledBy /* = 0 */)
{
	// Make any pending updates to the calculated frustum planes
	updateFrustumPlanes();

	// For each plane, see if all points are on the negative side
	// If so, object is not visible
	for (int plane = 0; plane < 6; ++plane)
	{
		// Skip far plane if infinite view frustum
		if (plane == FRUSTUM_PLANE_FAR && mFarDist == 0)
			continue;

		if (D3DXPlaneDotCoord(&mFrustumPlanes[plane], &vert) < 0.0f)
		{
			// ALL corners on negative side therefore out of view
			if (culledBy)
				*culledBy = (FrustumPlane)plane;
			return false;
		}

	}

	return true;
}

bool ElFrustum::isVisible(const D3DXVECTOR3& centre, const D3DXVECTOR3& halfSize, FrustumPlane* culledBy /* = 0 */)
{
	// Make any pending updates to the calculated frustum planes
	updateFrustumPlanes();

	// For each plane, see if all points are on the negative side
	// If so, object is not visible
	for (int plane = 0; plane < 6; ++plane)
	{
		// Skip far plane if infinite view frustum
		if (plane == FRUSTUM_PLANE_FAR && mFarDist == 0)
			continue;


		// Calculate the distance between box centre and the plane
		float dist = D3DXPlaneDotCoord(&mFrustumPlanes[plane], &centre);

		// Calculate the maximise allows absolute distance for
		// the distance between box centre and plane
		float maxAbsDist = fabs(mFrustumPlanes[plane].a * halfSize.x) +
			fabs(mFrustumPlanes[plane].b * halfSize.y) +
			fabs(mFrustumPlanes[plane].c * halfSize.z);

		if (dist < -maxAbsDist)
		{
			// ALL corners on negative side therefore out of view
			if (culledBy)
				*culledBy = (FrustumPlane)plane;
			return false;
		}
	}

	return true;
}

bool ElFrustum::isVisible(const ElAxisAlignedBox& bound, FrustumPlane* culledBy /* = 0 */)
{
	// Null boxes always invisible
	if (bound.isNull()) return false;

	// Infinite boxes always visible
	if (bound.isInfinite()) return true;

	// Get centre of the box
	D3DXVECTOR3 centre = bound.getCenter();
	// Get the half-size of the box
	D3DXVECTOR3 halfSize = bound.getHalfSize();

	return isVisible(centre, halfSize, culledBy);
}

bool ElFrustum::isFrustumOutOfDate()
{
	return mRecalcFrustum;
}

bool ElFrustum::isViewOutOfDate()
{
	// Attached to node?
	if (mParentNode)
	{
		if (mRecalcView ||
			mParentNode->getDerivedOrientation() != mLastParentOrientation ||
			mParentNode->getDerivedPosition() != mLastParentPosition)
		{
			// Ok, we're out of date with SceneNode we're attached to
			mLastParentOrientation = mParentNode->getDerivedOrientation();
			mLastParentPosition = mParentNode->getDerivedPosition();
			mRecalcView = true;
		}
	}

	return mRecalcView;
}

void ElFrustum::invalidateFrustum()
{
	mRecalcFrustum = true;
	mRecalcFrustumPlanes = true;
}

void ElFrustum::invalidateView()
{
	mRecalcView = true;
	mRecalcFrustumPlanes = true;
}

void ElFrustum::updateFrustum()
{
	if (isFrustumOutOfDate())
		updateFrustumImp();
}

void ElFrustum::updateFrustumImp()
{
	if (mProjType == PT_PERSPECTIVE)
		D3DXMatrixPerspectiveFovLH(&mProjMatrix, mFOV, mAspect, mNearDest, mFarDist);
	else
		D3DXMatrixOrthoLH(&mProjMatrix, mWidth, mHeight, mNearDest, mFarDist);

	mRecalcFrustum = false;

	// Signal to update frustum clipping planes
	mRecalcFrustumPlanes = true;
}

void ElFrustum::updateView()
{
	if (isViewOutOfDate())
		updateViewImp();
}

void ElFrustum::updateViewImp()
{
	const D3DXQUATERNION& orientation = getOrientationForViewUpdate();
	const D3DXVECTOR3& position = getPositionForViewUpdate();

	D3DXMATRIX rot;
	D3DXMatrixRotationQuaternion(&rot, &orientation);
	D3DXMatrixInverse(&rot, NULL, &rot);
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, position.x, position.y, position.z);
	D3DXMatrixInverse(&trans, NULL, &trans);
	
	// Make final combo
	mViewMatrix = trans * rot;

	// Deal with reflections
	if (mReflect)
		mViewMatrix = mReflectMatrix * mViewMatrix;

	mRecalcView = false;

	// Signal to update frustum clipping planes
	mRecalcFrustumPlanes = true;
}

void ElFrustum::updateFrustumPlanes()
{
	updateFrustum();
	updateView();

	if (mRecalcFrustumPlanes)
		updateFrustumPlanesImp();
}

void ElFrustum::updateFrustumPlanesImp()
{
	// Update the frustum planes
	D3DXMATRIX combo = mViewMatrix * mProjMatrix;

	mFrustumPlanes[FRUSTUM_PLANE_NEAR].a = combo._14 + combo._13;
	mFrustumPlanes[FRUSTUM_PLANE_NEAR].b = combo._24 + combo._23;
	mFrustumPlanes[FRUSTUM_PLANE_NEAR].c = combo._34 + combo._33;
	mFrustumPlanes[FRUSTUM_PLANE_NEAR].d = combo._44 + combo._43;

	mFrustumPlanes[FRUSTUM_PLANE_FAR].a = combo._14 - combo._13;
	mFrustumPlanes[FRUSTUM_PLANE_FAR].b = combo._24 - combo._23;
	mFrustumPlanes[FRUSTUM_PLANE_FAR].c = combo._34 - combo._33;
	mFrustumPlanes[FRUSTUM_PLANE_FAR].d = combo._44 - combo._43;

	mFrustumPlanes[FRUSTUM_PLANE_LEFT].a = combo._14 + combo._11;
	mFrustumPlanes[FRUSTUM_PLANE_LEFT].b = combo._24 + combo._21;
	mFrustumPlanes[FRUSTUM_PLANE_LEFT].c = combo._34 + combo._31;
	mFrustumPlanes[FRUSTUM_PLANE_LEFT].d = combo._44 + combo._41;

	mFrustumPlanes[FRUSTUM_PLANE_RIGHT].a = combo._14 - combo._11;
	mFrustumPlanes[FRUSTUM_PLANE_RIGHT].b = combo._24 - combo._21;
	mFrustumPlanes[FRUSTUM_PLANE_RIGHT].c = combo._34 - combo._31;
	mFrustumPlanes[FRUSTUM_PLANE_RIGHT].d = combo._44 - combo._41;

	mFrustumPlanes[FRUSTUM_PLANE_TOP].a = combo._14 - combo._12;
	mFrustumPlanes[FRUSTUM_PLANE_TOP].b = combo._24 - combo._22;
	mFrustumPlanes[FRUSTUM_PLANE_TOP].c = combo._34 - combo._32;
	mFrustumPlanes[FRUSTUM_PLANE_TOP].d = combo._44 - combo._42;

	mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].a = combo._14 + combo._12;
	mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].b = combo._24 + combo._22;
	mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].c = combo._34 + combo._32;
	mFrustumPlanes[FRUSTUM_PLANE_BOTTOM].d = combo._44 + combo._42;

	// Normalise each plane
	for (int i = 0; i < 6; ++i) 
		D3DXPlaneNormalize(&mFrustumPlanes[i], &mFrustumPlanes[i]);

	mRecalcFrustumPlanes = false;
}

const D3DXVECTOR3& ElFrustum::getPositionForViewUpdate() const
{
	return mLastParentPosition;
}

const D3DXQUATERNION& ElFrustum::getOrientationForViewUpdate() const
{
	return mLastParentOrientation;
}



