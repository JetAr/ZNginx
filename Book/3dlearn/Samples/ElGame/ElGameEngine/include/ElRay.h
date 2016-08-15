#ifndef __ElRay_H__
#define __ElRay_H__

#include "ElD3DHeaders.h"
#include "ElAxisAlignedBox.h"
#include <utility>

class ElRay
{
public:
	ElRay() : mOrigin(D3DXVECTOR3(0.0f, 0.0f, 0.0f)), mDirection(D3DXVECTOR3(0.0f, 0.0f, 1.0f)) {}
	ElRay(const D3DXVECTOR3& origin, const D3DXVECTOR3& direction) : mOrigin(origin), mDirection(direction) {}

	void setOrigin(const D3DXVECTOR3& origin) { mOrigin = origin; }
	const D3DXVECTOR3& getOrigin() const { return mOrigin; }

	void setDirection(const D3DXVECTOR3& dir) { mDirection = dir; }
	const D3DXVECTOR3& getDirection() const { return mDirection; }

	D3DXVECTOR3 getPoint(float t) const
	{ 
		return D3DXVECTOR3(mOrigin + (mDirection * t));
	}

	D3DXVECTOR3 operator*(float t) const
	{
		return getPoint(t);
	};

	std::pair<bool, float> intersects(const ElAxisAlignedBox& box) const;
	bool intersects(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, float& t, float& u, float& v) const;

protected:
	D3DXVECTOR3		mOrigin;
	D3DXVECTOR3		mDirection;
};

#endif //__ElRay_H__