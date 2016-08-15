#include "ElApplicationPCH.h"
#include "ElRaySceneQuery.h"
#include "ElSceneManager.h"
#include <algorithm>

ElRaySceneQuery::ElRaySceneQuery(ElSceneManager* mgr)
: mSortByDistance(true)
, mMaxResults(0)
, mParentSceneMgr(mgr)
{

}

ElRaySceneQuery::~ElRaySceneQuery()
{

}

void ElRaySceneQuery::setRay(const ElRay& ray)
{
	mRay = ray;
}

const ElRay& ElRaySceneQuery::getRay() const
{
	return mRay;
}

void ElRaySceneQuery::setSortByDistance(bool sort, unsigned short maxresults /* = 0 */)
{
	mSortByDistance = sort;
	mMaxResults = maxresults;
}

bool ElRaySceneQuery::getSortByDistance() const
{
	return mSortByDistance;
}

unsigned short ElRaySceneQuery::getMaxResults() const
{
	return mMaxResults;
}

ElRaySceneQueryResult& ElRaySceneQuery::execute()
{
	// Clear without freeing the vector buffer
	mResult.clear();

	mParentSceneMgr->intersects(*this);

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

ElRaySceneQueryResult& ElRaySceneQuery::getLastResults()
{
	return mResult;
}

void ElRaySceneQuery::clearResults()
{
	// C++ idiom to free vector buffer: swap with empty vector
	ElRaySceneQueryResult().swap(mResult);
}

bool ElRaySceneQuery::queryResult(ElMovableObject* obj, float distance)
{
	// Add to internal list
	ElRaySceneQueryResultEntry dets;
	dets.distance = distance;
	dets.movable = obj;
	mResult.push_back(dets);
	// Continue
	return true;
}