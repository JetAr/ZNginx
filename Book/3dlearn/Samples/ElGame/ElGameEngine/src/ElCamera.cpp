#include "ElApplicationPCH.h"
#include "ElCamera.h"
#include "ElSceneNode.h"

ElCamera::ElCamera()
: mRight(1.0f, 0.0f, 0.0f)
, mUp(0.0f, 1.0f, 0.0f)
, mLook(0.0f, 0.0f, 1.0f)
, mPosition(0.0f, 25.0f, 0.0f)
, mYawFixed(true)
, mDerivedPosition(0.0f, 0.0f, 0.0f)
, mRealPosition(0.0f, 0.0f, 0.0f)
{
	D3DXQuaternionIdentity(&mOrientation);
	D3DXQuaternionIdentity(&mDerivedOrientation);
	D3DXQuaternionIdentity(&mRealOrientation);

	invalidateFrustum();
	invalidateView();
}

ElCamera::~ElCamera()
{

}

void ElCamera::walk(float unit)
{
	if (mYawFixed)
		mPosition += D3DXVECTOR3(mLook.x, 0.0f, mLook.z) * unit;
	else
		mPosition += mLook * unit;

	invalidateView();
}

void ElCamera::strafe(float unit)
{
	if (mYawFixed)
		mPosition += D3DXVECTOR3(mRight.x, 0.0f, mRight.z) * unit;
	else
		mPosition += mRight * unit;
	
	invalidateView();
}

void ElCamera::fly(float unit)
{
	if (mYawFixed)
		mPosition.y += unit;
	else
		mPosition += mUp * unit;

	invalidateView();
}

void ElCamera::roll(float angle)
{
	// only roll if yaw-axis not fixed
	if (!mYawFixed)
	{
		D3DXMATRIX m;
		D3DXMatrixRotationAxis(&m, &mLook, angle);

		// rotate up and right around look vector
		D3DXVec3TransformCoord(&mUp, &mUp, &m);
		D3DXVec3TransformCoord(&mRight, &mRight, &m);
	}
	
	calcViewOrientation();
	invalidateView();
}

void ElCamera::yaw(float angle)
{
	D3DXMATRIX m;

	// rotate around world y-axis (0, 1, 0) if yaw-axis fixed
	if (mYawFixed)
	{
		D3DXMatrixRotationY(&m, angle);
	}
	else	// else rotate around own up vector
	{
		D3DXMatrixRotationAxis(&m, &mUp, angle);
	}

	// rotate right and look around up or y-axis
	D3DXVec3TransformCoord(&mRight, &mRight, &m);
	D3DXVec3TransformCoord(&mLook, &mLook,  &m);

	calcViewOrientation();
	invalidateView();
}

void ElCamera::pitch(float angle)
{
	D3DXMATRIX m;
	D3DXMatrixRotationAxis(&m, &mRight, angle);

	// rotate up and look around right vector
	D3DXVec3TransformCoord(&mUp, &mUp, &m);
	D3DXVec3TransformCoord(&mLook, &mLook, &m);

	calcViewOrientation();
	invalidateView();
}

void ElCamera::setPosition(const D3DXVECTOR3& pos)
{
	mPosition = pos;
	invalidateView();
}

void ElCamera::setFixedYawAxis(bool fixed)
{
	mYawFixed = fixed;
}

void ElCamera::lookAt(const D3DXVECTOR3& targetPoint)
{
	updateView();
	setDirection(targetPoint - mRealPosition);
}

void ElCamera::setDirection(const D3DXVECTOR3& vec)
{
	// Do nothing if given a zero vector
	if (vec == D3DXVECTOR3(0.0f, 0.0f, 0.0f))
		return;

	D3DXVECTOR3 adjustVec = vec;
	if (mYawFixed)
		adjustVec.y = 0.0f;
	D3DXVec3Normalize(&adjustVec, &adjustVec);
	
	D3DXMATRIX m;
	if (D3DXVec3LengthSq(&(D3DXVECTOR3(0.0f, 0.0f, 1.0f) + adjustVec)) < 0.00005f)
	{
		// Oops, a 180 degree turn (infinite possible rotation axes)
		// Default to yaw i.e. use current DOWN
		D3DXMatrixRotationAxis(&m, &D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DX_PI);
	}
	else
	{
		D3DXVECTOR3 axe;
		D3DXVec3Cross(&axe, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), &adjustVec);

		float dot = D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &adjustVec);
		D3DXMatrixRotationAxis(&m, &axe, acos(dot));
	}

	mLook = vec;
	D3DXVec3TransformCoord(&mUp, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &m);
	D3DXVec3TransformCoord(&mRight, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &m);

	calcViewOrientation();
	invalidateView();
}

const D3DXVECTOR3& ElCamera::getDirection() const
{
	return mLook;
}

const D3DXVECTOR3& ElCamera::getUp() const
{
	return mUp;
}

const D3DXQUATERNION& ElCamera::getDerivedOrientation()
{
	updateView();
	return mDerivedOrientation;
}

const D3DXVECTOR3& ElCamera::getDerivedPosition()
{
	updateView();
	return mDerivedPosition;
}

const D3DXQUATERNION& ElCamera::getRealOrientation()
{
	updateView();
	return mRealOrientation;
}

const D3DXVECTOR3& ElCamera::getRealPosition()
{
	updateView();
	return mRealPosition;
}

ElRay ElCamera::getCameraToViewportRay(float screenx, float screeny)
{
	D3DXMATRIX proj = getProjectionMatrix();
	D3DXMATRIX view = getViewMatrix();

	// Compute the vector of the Pick ray in screen space
	D3DXVECTOR3 v;
	v.x = ((2.0f * screenx) - 1.0f) / proj._11;
	v.y = (1.0f - (2.0f * screeny)) / proj._22;
	v.z = 1.0f;

	// Transform the screen space Pick ray into 3D space
	D3DXMATRIX m;
	D3DXMatrixInverse(&m, NULL, &view);
	
	D3DXVECTOR3 origin, direction;
	direction.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	direction.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	direction.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	origin.x = m._41;
	origin.y = m._42;
	origin.z = m._43;

	return ElRay(origin, direction);
}

bool ElCamera::isViewOutOfDate()
{
	// Overridden from Frustum to use local orientation / position offsets
	// Attached to node?
	if (mParentNode != 0)
	{
		if (mRecalcView ||
			mParentNode->getDerivedOrientation() != mLastParentOrientation ||
			mParentNode->getDerivedPosition() != mLastParentPosition)
		{
			// Ok, we're out of date with SceneNode we're attached to
			mLastParentOrientation = mParentNode->getDerivedOrientation();
			mLastParentPosition = mParentNode->getDerivedPosition();

			mRealOrientation = mLastParentOrientation * mOrientation;
			D3DXMATRIX m;
			D3DXMatrixRotationQuaternion(&m, &mLastParentOrientation);
			D3DXVec3TransformCoord(&mRealPosition, &mPosition, &m);
			mRealPosition += mLastParentPosition;
			
			mRecalcView = true;
		}
	}
	else
	{
		// Rely on own updates
		mRealOrientation = mOrientation;
		mRealPosition = mPosition;
	}

	// Deriving reflected orientation / position
	if (mRecalcView)
	{
		if (mReflect)
		{
			// Calculate reflected orientation, position
			D3DXMATRIX rot;
			D3DXMatrixRotationQuaternion(&rot, &mRealOrientation);
			D3DXMATRIX trans;
			D3DXMatrixTranslation(&trans, mRealPosition.x, mRealPosition.y, mRealPosition.z);
			
			D3DXMATRIX m = rot * trans * mReflectMatrix;

			D3DXVECTOR3 scale;
			D3DXMatrixDecompose(&scale, &mDerivedOrientation, &mDerivedPosition, &m);
		}
		else
		{
			mDerivedOrientation = mRealOrientation;
			mDerivedPosition = mRealPosition;
		}
	}

	return mRecalcView;
}

void ElCamera::calcViewOrientation()
{
	D3DXVec3Normalize(&mLook, &mLook);

	D3DXVec3Cross(&mUp, &mLook, &mRight);
	D3DXVec3Normalize(&mUp, &mUp);

	D3DXVec3Cross(&mRight, &mUp, &mLook);
	D3DXVec3Normalize(&mRight, &mRight);

	D3DXMATRIX rot;
	D3DXMatrixIdentity(&rot);

	rot._11 = mRight.x;
	rot._12 = mRight.y;
	rot._13 = mRight.z;

	rot._21 = mUp.x;
	rot._22 = mUp.y;
	rot._23 = mUp.z;

	rot._31 = mLook.x;
	rot._32 = mLook.y;
	rot._33 = mLook.z;

	D3DXQuaternionRotationMatrix(&mOrientation, &rot);

	// transform to parent space
	if (mParentNode)
	{
		D3DXQUATERNION parentOrientation = mParentNode->getDerivedOrientation();
		D3DXQuaternionInverse(&parentOrientation, &parentOrientation);
		mOrientation = parentOrientation * mOrientation;
	}
}

const D3DXVECTOR3& ElCamera::getPositionForViewUpdate() const
{
	// Note no update, because we're calling this from the update!
	return mRealPosition;
}

const D3DXQUATERNION& ElCamera::getOrientationForViewUpdate() const
{
	return mRealOrientation;
}






