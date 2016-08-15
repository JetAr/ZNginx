#include "ElSamplePCH.h"
#include "ElDefines.h"
#include "ElFps.h"
#include "ElDeviceManager.h"

ElFps::ElFps()
: mLastTime(0)
, mFrameCount(0)
, mSampleRate(500)
, mFps(0.0f)
{

}

ElFps::~ElFps()
{

}

void ElFps::update(TimeValue t)
{
	++mFrameCount;
	TimeValue escaped = t - mLastTime;
	if (escaped > mSampleRate)
	{
		mFps = (float)mFrameCount / ((float)escaped * 0.001f);
		mFrameCount = 0;
		mLastTime = t;
	}
}

float ElFps::getFps() const
{
	return mFps;
}
