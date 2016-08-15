#ifndef __ElRayTerrainQuery_H__
#define __ElRayTerrainQuery_H__

#include "ElRay.h"
#include "ElTerrain.h"
#include "ElD3DHeaders.h"
#include <vector>

struct ElRayTerrainQueryResultEntry
{
	// Distance along the ray
	float distance;
	// Single intersection point
	D3DXVECTOR3 singleIntersection;
	// Comparison operator for sorting
	bool operator < (const ElRayTerrainQueryResultEntry& rhs) const
	{
		return this->distance < rhs.distance;
	}
};
typedef std::vector<ElRayTerrainQueryResultEntry> ElRayTerrainQueryResult;

class ElRayTerrainQuery
{
public:
	ElRayTerrainQuery(ElTerrain* terrain);
	virtual ~ElRayTerrainQuery();

	virtual void setRay(const ElRay& ray);
	virtual const ElRay& getRay() const;
	virtual void setSortByDistance(bool sort, unsigned short maxresults = 0);
	virtual bool getSortByDistance() const;
	virtual unsigned short getMaxResults() const;

	virtual ElRayTerrainQueryResult& execute();
	virtual ElRayTerrainQueryResult& getLastResults();
	virtual void clearResults();
	virtual bool queryResult(const D3DXVECTOR3& intersection, float distance);

protected:
	ElRay					mRay;
	bool					mSortByDistance;
	unsigned short			mMaxResults;
	ElTerrain*				mParentTerrain;
	ElRayTerrainQueryResult	mResult;
};

#endif //__ElRayTerrainQuery_H__