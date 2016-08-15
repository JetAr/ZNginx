#ifndef __ElFps_H__
#define __ElFps_H__

#include "ElD3DHeaders.h"
#include "ElDefines.h"

class ElFps
{
public:
	ElFps();
	virtual ~ElFps();

	void update(TimeValue t);
	float getFps() const;

protected:
	TimeValue				mLastTime;
	TimeValue				mFrameCount;
	TimeValue				mSampleRate;

	float					mFps;
};

#endif //__ElFps_H__