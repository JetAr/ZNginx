#ifndef __ElRaySceneQuery_H__
#define __ElRaySceneQuery_H__

#include "ElRay.h"
#include "ElMovableObject.h"
#include <vector>

class ElSceneManager;

struct ElRaySceneQueryResultEntry
{
	// Distance along the ray
	float distance;
	// The movable, or NULL if this is not a movable result
	ElMovableObject* movable;
	// Comparison operator for sorting
	bool operator < (const ElRaySceneQueryResultEntry& rhs) const
	{
		return this->distance < rhs.distance;
	}
};
typedef std::vector<ElRaySceneQueryResultEntry> ElRaySceneQueryResult;

class ElRaySceneQuery
{
public:
	ElRaySceneQuery(ElSceneManager* mgr);
	virtual ~ElRaySceneQuery();

	virtual void setRay(const ElRay& ray);
	virtual const ElRay& getRay() const;
	virtual void setSortByDistance(bool sort, unsigned short maxresults = 0);
	virtual bool getSortByDistance() const;
	virtual unsigned short getMaxResults() const;

	virtual ElRaySceneQueryResult& execute();
	virtual ElRaySceneQueryResult& getLastResults();
	virtual void clearResults();
	virtual bool queryResult(ElMovableObject* obj, float distance);

protected:
	ElRay					mRay;
	bool					mSortByDistance;
	unsigned short			mMaxResults;
	ElSceneManager*			mParentSceneMgr;
	ElRaySceneQueryResult	mResult;
};

#endif //__ElRaySceneQuery_H__