#ifndef __ElAxisAlignedBox_H__
#define __ElAxisAlignedBox_H__

#include "ElD3DHeaders.h"
#include "assert.h"
#include <limits>

class ElAxisAlignedBox
{
	/*
	   5-----4
	  /|    /|
	 / |   / |
	1-----2  |
	|  6--|--7
	| /   | /
	|/    |/
	0-----3
	*/
public:
	enum Extent
	{
		EXTENT_NULL,
		EXTENT_FINITE,
		EXTENT_INFINITE
	};

public:
	inline ElAxisAlignedBox() : mMinimum(-0.5f, -0.5f, -0.5f), mMaximum(0.5f, 0.5f, 0.5f), mExtent(EXTENT_NULL) {}
	inline ElAxisAlignedBox(const D3DXVECTOR3& min, const D3DXVECTOR3& max) : mMinimum(-0.5f, -0.5f, -0.5f), mMaximum(0.5f, 0.5f, 0.5f), mExtent(EXTENT_NULL)
	{
		setExtents( min, max );
	}
	virtual ~ElAxisAlignedBox() {}

	inline const D3DXVECTOR3& getMinimum() const
	{
		return mMinimum;
	}
	inline const D3DXVECTOR3& getMaximum() const
	{
		return mMaximum;
	}

	inline void setNull()
	{
		mExtent = EXTENT_NULL;
	}
	inline void setMinimum(const D3DXVECTOR3& vec)
	{
		mExtent = EXTENT_FINITE;
		mMinimum = vec;
	}
	inline void setMaximum(const D3DXVECTOR3& vec)
	{
		mExtent = EXTENT_FINITE;
		mMaximum = vec;
	}
	inline void setExtents(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
	{
		assert((min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
			"The minimum corner of the box must be less than or equal to maximum corner");

		mExtent = EXTENT_FINITE;
		mMinimum = min;
		mMaximum = max;
	}

	inline void merge(const ElAxisAlignedBox& rhs)
	{
		// Do nothing if rhs null, or this is infinite
		if ((rhs.mExtent == EXTENT_NULL) || (mExtent == EXTENT_INFINITE))
		{
			return;
		}
		// Otherwise if rhs is infinite, make this infinite, too
		else if (rhs.mExtent == EXTENT_INFINITE)
		{
			mExtent = EXTENT_INFINITE;
		}
		// Otherwise if current null, just take rhs
		else if (mExtent == EXTENT_NULL)
		{
			setExtents(rhs.mMinimum, rhs.mMaximum);
		}
		// Otherwise merge
		else
		{
			D3DXVECTOR3 min = mMinimum;
			D3DXVECTOR3 max = mMaximum;
			D3DXVec3Minimize(&min, &min, &rhs.mMinimum);
			D3DXVec3Maximize(&max, &max, &rhs.mMaximum);

			setExtents(min, max);
		}
	}
	inline void merge(const D3DXVECTOR3& point)
	{
		switch (mExtent)
		{
		case EXTENT_NULL: // if null, use this point
			setExtents(point, point);
			return;

		case EXTENT_FINITE:
			D3DXVec3Minimize(&mMinimum, &mMinimum, &point);
			D3DXVec3Maximize(&mMaximum, &mMaximum, &point);
			return;

		case EXTENT_INFINITE: // if infinite, makes no difference
			return;
		}

		assert(false && "Never reached");
	}

	inline void transform(const D3DXMATRIX& matrix)
	{
		// Do nothing if current null or infinite
		if (mExtent != EXTENT_FINITE)
			return;

		D3DXVECTOR3 oldMin, oldMax, currentCorner, transCorner;

		// Getting the old values so that we can use the existing merge method.
		oldMin = mMinimum;
		oldMax = mMaximum;

		// reset
		setNull();

		// We sequentially compute the corners in the following order :
		// 0, 6, 5, 1, 2, 4, 7, 3
		// This sequence allows us to only change one member at a time to get at all corners.

		// For each one, we transform it using the matrix
		// Which gives the resulting point and merge the resulting point.

		// First corner
		// min min min
		currentCorner = oldMin;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// min min max
		currentCorner.z = oldMax.z;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// min max max
		currentCorner.y = oldMax.y;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// min max min
		currentCorner.z = oldMin.z;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// max max min
		currentCorner.x = oldMax.x;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// max max max
		currentCorner.z = oldMax.z;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// max min max
		currentCorner.y = oldMin.y;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);

		// max min min
		currentCorner.z = oldMin.z;
		D3DXVec3TransformCoord(&transCorner, &currentCorner, &matrix);
		merge(transCorner);
	}
	inline void transformAffine(const D3DXMATRIX& m)
	{
		// Do nothing if current null or infinite
		if (mExtent != EXTENT_FINITE)
			return;

		D3DXVECTOR3 centre = getCenter();
		D3DXVECTOR3 halfSize = getHalfSize();

		D3DXVECTOR3 newCentre;
		D3DXVec3TransformCoord(&newCentre, &centre, &m);
		D3DXVECTOR3 newHalfSize(
			fabs(m._11) * halfSize.x + fabs(m._21) * halfSize.y + fabs(m._31) * halfSize.z,
			fabs(m._12) * halfSize.x + fabs(m._22) * halfSize.y + fabs(m._32) * halfSize.z,
			fabs(m._13) * halfSize.x + fabs(m._23) * halfSize.y + fabs(m._33) * halfSize.z);

		setExtents(newCentre - newHalfSize, newCentre + newHalfSize);
	}

	inline bool isNull() const
	{
		return mExtent == EXTENT_NULL;
	}
	inline bool isFinite() const
	{
		return mExtent == EXTENT_FINITE;
	}
	inline void setInfinite()
	{
		mExtent = EXTENT_INFINITE;
	}
	inline bool isInfinite() const
	{
		return mExtent == EXTENT_INFINITE;
	}

	inline bool intersects(const ElAxisAlignedBox& b2) const
	{
		// Early-fail for nulls
		if (this->isNull() || b2.isNull())
			return false;

		// Early-success for infinites
		if (this->isInfinite() || b2.isInfinite())
			return true;

		// Use up to 6 separating planes
		if (mMaximum.x < b2.mMinimum.x)
			return false;
		if (mMaximum.y < b2.mMinimum.y)
			return false;
		if (mMaximum.z < b2.mMinimum.z)
			return false;

		if (mMinimum.x > b2.mMaximum.x)
			return false;
		if (mMinimum.y > b2.mMaximum.y)
			return false;
		if (mMinimum.z > b2.mMaximum.z)
			return false;

		// otherwise, must be intersecting
		return true;

	}
	inline ElAxisAlignedBox intersection(const ElAxisAlignedBox& b2) const
	{
		if (this->isNull() || b2.isNull())
		{
			return ElAxisAlignedBox();
		}
		else if (this->isInfinite())
		{
			return b2;
		}
		else if (b2.isInfinite())
		{
			return *this;
		}

		D3DXVECTOR3 intMin = mMinimum;
		D3DXVECTOR3 intMax = mMaximum;

		D3DXVec3Minimize(&intMin, &intMin, &b2.getMinimum());
		D3DXVec3Maximize(&intMax, &intMax, &b2.getMaximum());

		// Check intersection isn't null
		if (intMin.x < intMax.x &&
			intMin.y < intMax.y &&
			intMin.z < intMax.z)
		{
			return ElAxisAlignedBox(intMin, intMax);
		}

		return ElAxisAlignedBox();
	}

	inline D3DXVECTOR3 getCenter() const
	{
		assert((mExtent == EXTENT_FINITE) && "Can't get center of a null or infinite AAB");

		return D3DXVECTOR3(
			(mMaximum.x + mMinimum.x) * 0.5f,
			(mMaximum.y + mMinimum.y) * 0.5f,
			(mMaximum.z + mMinimum.z) * 0.5f);
	}
	inline D3DXVECTOR3 getSize() const
	{
		switch (mExtent)
		{
		case EXTENT_NULL:
			return D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		case EXTENT_FINITE:
			return mMaximum - mMinimum;

		case EXTENT_INFINITE:
			return D3DXVECTOR3(
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity());

		default: // shut up compiler
			assert(false && "Never reached");
			return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}
	inline D3DXVECTOR3 getHalfSize() const
	{
		switch (mExtent)
		{
		case EXTENT_NULL:
			return D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		case EXTENT_FINITE:
			return (mMaximum - mMinimum) * 0.5;

		case EXTENT_INFINITE:
			return D3DXVECTOR3(
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity(),
				std::numeric_limits<float>::infinity());

		default: // shut up compiler
			assert(false && "Never reached");
			return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}

protected:
	D3DXVECTOR3		mMinimum;
	D3DXVECTOR3		mMaximum;
	Extent			mExtent;
};

#endif //__ElAxisAlignedBox_H__