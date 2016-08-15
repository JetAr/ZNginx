#ifndef _GUTTIMER_
#define _GUTTIMER_

class GutTimer
{
public:
	enum TimerState
	{
		GUTTIMER_RUNNING,
		GUTTIMER_STOPPED,
	};

	float m_fTime;
	TimerState m_eState;
	LARGE_INTEGER m_PerformanceCount;

public:
	GutTimer(void);
	float Start(void);
	float Stop(void);
	void  Restart(void);

	float GetTime_MS(void);
	float GetTime_Second(void);
	float GetTime(void) { return GetTime_Second(); }
};

#endif // _GUTTIMER_