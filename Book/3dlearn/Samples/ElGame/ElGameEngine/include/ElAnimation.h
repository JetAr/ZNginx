#ifndef __ElAnimation_H__
#define __ElAnimation_H__

#include "ElD3DHeaders.h"
#include "ElDefines.h"
#include <vector>
#include <string>

struct ElAnimationState
{
	int		startIdx;
	int		endIdx;
	float	scale;

	ElAnimationState() : startIdx(0), endIdx(0), scale(0.0f) {}
};

class ElAnimation
{
public:
	typedef std::vector<TimeValue> KeyFrameTimeList;

public:
	ElAnimation();
	virtual ~ElAnimation();

	void reset();
	void addKeyFrameTime(TimeValue timeKey);
	int getFrameTimeCount();
	
	void play(TimeValue startTime, bool loop = true);
	void update(TimeValue currentTime);
	void stop();
	void getCurrentState(ElAnimationState& as);

	void setName(const std::string& name);
	const std::string& getName();
	void setSpeed(float speed);
	float getSpeed();

protected:
	KeyFrameTimeList	mKeyFrameTimes;
	
	bool				mLoop;
	bool				mPlaying;
	TimeValue			mStartTime;
	TimeValue			mPastTime;
	float				mSpeed;

	std::string			mName;
};

#endif //__ElAnimation_H__
