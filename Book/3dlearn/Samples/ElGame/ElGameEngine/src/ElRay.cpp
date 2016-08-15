#include "ElApplicationPCH.h"
#include "ElRay.h"

std::pair<bool, float> ElRay::intersects(const ElAxisAlignedBox& box) const
{
	if (box.isNull()) return std::pair<bool, float>(false, 0);
	if (box.isInfinite()) return std::pair<bool, float>(true, 0);

	float lowt = 0.0f;
	float t;
	bool hit = false;
	D3DXVECTOR3 hitpoint;
	const D3DXVECTOR3& min = box.getMinimum();
	const D3DXVECTOR3& max = box.getMaximum();
	const D3DXVECTOR3& rayorig = getOrigin();
	const D3DXVECTOR3& raydir = getDirection();

	// Check origin inside first
	if (rayorig.x > min.x && rayorig.y > min.y && rayorig.z > min.z &&
		rayorig.x < max.x && rayorig.y < max.y && rayorig.z < max.z)
	{
		return std::pair<bool, float>(true, 0);
	}

	// Check each face in turn, only check closest 3
	// Min x
	if (rayorig.x <= min.x && raydir.x > 0)
	{
		t = (min.x - rayorig.x) / raydir.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max x
	if (rayorig.x >= max.x && raydir.x < 0)
	{
		t = (max.x - rayorig.x) / raydir.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min y
	if (rayorig.y <= min.y && raydir.y > 0)
	{
		t = (min.y - rayorig.y) / raydir.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max y
	if (rayorig.y >= max.y && raydir.y < 0)
	{
		t = (max.y - rayorig.y) / raydir.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min z
	if (rayorig.z <= min.z && raydir.z > 0)
	{
		t = (min.z - rayorig.z) / raydir.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max z
	if (rayorig.z >= max.z && raydir.z < 0)
	{
		t = (max.z - rayorig.z) / raydir.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	return std::pair<bool, float>(hit, lowt);
}

bool ElRay::intersects(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, float& t, float& u, float& v) const
{
	// Find vectors for two edges sharing vert0
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec, &mDirection, &edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = D3DXVec3Dot(&edge1, &pvec);

	D3DXVECTOR3 tvec;
	if (det > 0)
		tvec = mOrigin - v0;
	else
	{
		tvec = v0 - mOrigin;
		det = -det;
	}

	if (det < 0.0001f)
		return false;

	// Calculate U parameter and test bounds
	u = D3DXVec3Dot(&tvec, &pvec);
	if (u < 0.0f || u > det)
		return false;

	// Prepare to test V parameter
	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	v = D3DXVec3Dot(&mDirection, &qvec);
	if (v < 0.0f || u + v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	t = D3DXVec3Dot(&edge2, &qvec);
	float fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;

	return true;
}