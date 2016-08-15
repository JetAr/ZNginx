#ifndef __ElCamera_H__
#define __ElCamera_H__

#include "ElD3DHeaders.h"
#include "ElFrustum.h"
#include "ElRay.h"

class ElCamera : public ElFrustum
{
public:
	ElCamera();
	virtual ~ElCamera();

	void walk(float unit);
	void strafe(float unit);
	void fly(float unit);
	
	void roll(float angle);
	void yaw(float angle);
	void pitch(float angle);

	void setPosition(const D3DXVECTOR3& pos);
	void setFixedYawAxis(bool fixed);

	void lookAt(const D3DXVECTOR3& targetPoint);
	void setDirection(const D3DXVECTOR3& vec);

	const D3DXVECTOR3& getDirection() const;
	const D3DXVECTOR3& getUp() const;

	const D3DXQUATERNION& getDerivedOrientation();
	const D3DXVECTOR3& getDerivedPosition();

	const D3DXQUATERNION& getRealOrientation();
	const D3DXVECTOR3& getRealPosition();

	ElRay getCameraToViewportRay(float screenx, float screeny);

protected:
	bool isViewOutOfDate();
	void calcViewOrientation();

	const D3DXVECTOR3& getPositionForViewUpdate() const;
	const D3DXQUATERNION& getOrientationForViewUpdate() const;

protected:
	D3DXVECTOR3			mRight;
	D3DXVECTOR3			mUp;
	D3DXVECTOR3			mLook;
	D3DXVECTOR3			mPosition;
	
	bool				mYawFixed;

	D3DXQUATERNION		mOrientation;
	D3DXQUATERNION		mDerivedOrientation;
	D3DXVECTOR3			mDerivedPosition;
	D3DXQUATERNION		mRealOrientation;
	D3DXVECTOR3			mRealPosition;
};

#endif //__ElCamera_H__