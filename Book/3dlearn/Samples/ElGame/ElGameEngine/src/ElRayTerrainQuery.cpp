#include "ElApplicationPCH.h"
#include "ElRayTerrainQuery.h"
#include <algorithm>

ElRayTerrainQuery::ElRayTerrainQuery(ElTerrain* terrain)
: mSortByDistance(true)
, mMaxResults(0)
, mParentTerrain(terrain)
{

}

ElRayTerrainQuery::~ElRayTerrainQuery()
{

}

void ElRayTerrainQuery::setRay(const ElRay& ray)
{
	mRay = ray;
}

const ElRay& ElRayTerrainQuery::getRay() const
{
	return mRay;
}

void ElRayTerrainQuery::setSortByDistance(bool sort, unsigned short maxresults /* = 0 */)
{
	mSortByDistance = sort;
	mMaxResults = maxresults;
}

bool ElRayTerrainQuery::getSortByDistance() const
{
	return mSortByDistance;
}

unsigned short ElRayTerrainQuery::getMaxResults() const
{
	return mMaxResults;
}

ElRayTerrainQueryResult& ElRayTerrainQuery::execute()
{
	// Clear without freeing the vector buffer
	mResult.clear();

	if (mParentTerrain)
		mParentTerrain->intersects(*this);

	if (mSortByDistance)
	{
		if (mMaxResults != 0 && mMaxResults < mResult.size())
		{
			// Partially sort the N smallest elements, discard others
			std::partial_sort(mResult.begin(), mResult.begin() + mMaxResults, mResult.end());
			mResult.resize(mMaxResults);
		}
		else
		{
			// Sort entire result array
			std::sort(mResult.begin(), mResult.end());
		}
	}

	return mResult;
}

ElRayTerrainQueryResult& ElRayTerrainQuery::getLastResults()
{
	return mResult;
}

void ElRayTerrainQuery::clearResults()
{
	// C++ idiom to free vector buffer: swap with empty vector
	ElRayTerrainQueryResult().swap(mResult);
}

bool ElRayTerrainQuery::queryResult(const D3DXVECTOR3& intersection, float distance)
{
	// Add to internal list
	ElRayTerrainQueryResultEntry dets;
	dets.distance = distance;
	dets.singleIntersection = intersection;
	mResult.push_back(dets);
	// Continue
	return true;
}