#include "ElApplicationPCH.h"
#include "ElAnimation.h"

ElAnimation::ElAnimation()
: mLoop(true)
, mPlaying(false)
, mStartTime(0)
, mPastTime(0)
, mSpeed(1.0f)
{

}

ElAnimation::~ElAnimation()
{

}

void ElAnimation::reset()
{
	mLoop = true;
	mPlaying = false;
	mStartTime = 0;
	mPastTime = 0;
	mSpeed = 1.0f;
	mKeyFrameTimes.clear();
}

void ElAnimation::addKeyFrameTime(TimeValue timeKey)
{
	mKeyFrameTimes.push_back(timeKey);
}

int ElAnimation::getFrameTimeCount()
{
	return mKeyFrameTimes.size();
}

void ElAnimation::play(TimeValue startTime, bool loop /* = true */)
{
	mStartTime = startTime;
	mLoop = loop;
	mPlaying = true;
}

void ElAnimation::update(TimeValue currentTime)
{
	if (mPlaying)
	{
		mPastTime = (currentTime - mStartTime);
		
		if (!mKeyFrameTimes.empty() && mLoop)
		{
			TimeValue overallTime = mKeyFrameTimes.at(mKeyFrameTimes.size() - 1);
			while (mPastTime > overallTime / mSpeed)
				mPastTime -= overallTime / mSpeed;
		}
	}
}

void ElAnimation::stop()
{
	mPlaying = false;
}

void ElAnimation::getCurrentState(ElAnimationState& as)
{
	as.startIdx = as.endIdx = 0;
	as.scale = 0.0f;

	int numFrame = mKeyFrameTimes.size();
	int max = numFrame - 1;
	bool hit = false;
	for (int i = 0; i < max; ++i)
	{
		float timeStart = mKeyFrameTimes[i] / mSpeed;
		float timeEnd = mKeyFrameTimes[i + 1] / mSpeed;

		if (mPastTime >= timeStart && mPastTime <= timeEnd)
		{
			as.startIdx = i;
			as.endIdx = i + 1;
			as.scale = ((float)mPastTime - timeStart) / (timeEnd - timeStart);

			hit = true;
		}
	}

	if (!hit && numFrame > 0)
	{
		as.startIdx = as.endIdx = max;
		as.scale = 0.0f;
	}
}

void ElAnimation::setName(const std::string& name)
{
	mName = name;
}

const std::string& ElAnimation::getName()
{
	return mName;
}

void ElAnimation::setSpeed(float speed)
{
	if (!float_equal(mSpeed, speed))
	{
		if (mPlaying)
		{
			TimeValue newPastTime = mPastTime * (speed / mSpeed);
			mStartTime = mStartTime + (mPastTime - newPastTime);
		}

		mSpeed = speed;
	}
}

float ElAnimation::getSpeed()
{
	return mSpeed;
}

